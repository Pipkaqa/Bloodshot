#include "Serialization/Archive.h"
#include "AssertionMacros.h"

namespace Bloodshot
{
	enum class ETokenType : uint8_t
	{
		StringLiteral = 0,
		NumberLiteral,
		Separator,
	};

	struct FToken final
	{
		FString Value;
		ETokenType Type = ETokenType::NumberLiteral;
	};

	class FTokenizer final
	{
	public:
		TVector<FToken> Tokenize(FStringView Source)
		{
			this->Source = Source;
			Position = 0;

			TVector<FToken> Result;

			while (Position < Source.size())
			{
				const char CurrentChar = Get();

				if (std::isspace(CurrentChar))
				{
					++Position;
					continue;
				}

				FToken Token;
				FString& TokenValue = Token.Value;
				ETokenType& TokenType = Token.Type;

				if (CurrentChar == ':' ||
					CurrentChar == '[' ||
					CurrentChar == ']' ||
					CurrentChar == ',')
				{
					TokenValue = Consume();
					TokenType = ETokenType::Separator;
				}
				else if (CurrentChar == '-' || CurrentChar == '.' || std::isdigit(CurrentChar))
				{
					TokenType = ETokenType::NumberLiteral;

					while (Position < Source.size() && (std::isdigit(Get()) || Get() == '.' || Get() == '-'))
					{
						TokenValue += Consume();
					}
				}
				else if (CurrentChar == '"')
				{
					TokenValue += Consume();
					TokenType = ETokenType::StringLiteral;

					while (Position < Source.size() && Get() != '"')
					{
						TokenValue += Consume();
					}

					if (Position < Source.size())
					{
						TokenValue += Consume();
					}
				}

				Result.emplace_back(std::move(Token));
			}

			return Result;
		}

	private:
		FStringView Source;
		size_t Position = 0;

		char Get() const
		{
			return Source.at(Position);
		}

		char Consume()
		{
			return Source.at(Position++);
		}

		char Peek() const
		{
			return (Position + 1 < Source.size()) ? Source.at(Position + 1) : '\0';
		}
	};

	class FParser final
	{
	public:
		TUnorderedMap<FString, FEncodedNode> Parse(const TVector<FToken>& Tokens)
		{
			this->Tokens = &Tokens;

			TUnorderedMap<FString, FEncodedNode> Result;

			auto GetUnquotedValue = [this]() -> FString
				{
					const FToken& Token = Consume();
					const FString& Value = Token.Value;
					return Value.substr(1, Value.length() - 2);
				};

			while (TokenIndex < Tokens.size())
			{
				if (Get().Type == ETokenType::StringLiteral)
				{
					const FToken& Token = Consume();
					const FString& Value = Token.Value;
					FString UnquotedValue = Value.substr(1, Value.length() - 2);

					FEncodedNode& NewNode = Result[std::move(UnquotedValue)];

					if (Consume().Value != ":")
					{
						BS_LOG(Fatal, "Failed to parse data: Token \":\" is required after \"...\" in top level");
					}

					FString& NewNodeValue = NewNode.Value;

					if (Get().Type == ETokenType::StringLiteral)
					{
						NewNodeValue = Consume().Value;
					}
					else if (Get().Type == ETokenType::NumberLiteral)
					{
						NewNodeValue = Consume().Value;
					}
					else if (Get().Value == "[")
					{
						NewNode.Children = ParseSequence();
					}
					else
					{
						BS_LOG(Fatal,
							"Failed to parse data: Expected after token \":\": string literal or number literal or sequence begin");
					}
				}
			}

			return Result;
		}

	private:
		const TVector<FToken>* Tokens;
		size_t TokenIndex = 0;

		TVector<FEncodedNode> ParseSequence()
		{
			++TokenIndex;

			TVector<FEncodedNode> Result;

			while (Get().Value != "]")
			{
				if (Get().Value == "[")
				{
					Result.emplace_back().Children = ParseSequence();
				}
				else if (Get().Type == ETokenType::StringLiteral)
				{
					FEncodedNode& NewNode = Result.emplace_back();
					NewNode.Value = Consume().Value;
				}
				else if (Get().Type == ETokenType::NumberLiteral)
				{
					FEncodedNode& NewNode = Result.emplace_back();
					NewNode.Value = Consume().Value;
				}
				else
				{
					++TokenIndex;
				}
			}

			++TokenIndex;

			return Result;
		}

		const FToken& Get() const
		{
			return Tokens->at(TokenIndex);
		}

		const FToken& Consume()
		{
			return Tokens->at(TokenIndex++);
		}
	};

	FArchive::FArchive(const std::filesystem::path& OutputPath)
		: FilePath(OutputPath)
		, Stream(OutputPath, std::ios_base::out | std::ios_base::app)
	{
		Stream.close();
		Stream.open(OutputPath, std::ios_base::in | std::ios_base::out | std::ios_base::app);

		std::stringstream StringStream;
		StringStream << Stream.rdbuf();

		const FString& Content = StringStream.str();
		const TVector<FToken>& Tokens = FTokenizer().Tokenize(Content);

		NodeTree = FParser().Parse(Tokens);
	}

	FArchive::~FArchive()
	{
		if (bNeedFlush) Flush();
		Stream.close();
	}

	void FArchive::Flush()
	{
		Stream.close();
		Stream.open(FilePath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

		for (const TPair<const FString, FEncodedNode> NodePair : NodeTree)
		{
			const FString& Name = NodePair.first;
			const FEncodedNode& Node = NodePair.second;

			Write("\"{}\": ", Name);
			WriteNodeDataRecursive(Node);
			NewLine();
		}

		Stream << Buffer;
		Buffer.clear();
		bNeedFlush = false;
	}

	void FArchive::WriteNodeDataRecursive(const FEncodedNode& Node)
	{
		// BSTODO: Replace recursion to loop

		if (Node.Children.size())
		{
			Write("[");
			PushScope();
			NewLine();

			for (const FEncodedNode& Child : Node.Children)
			{
				WriteNodeDataRecursive(Child);
				Write(",");
				NewLine();
			}

			Undo();
			Undo();
			PopScope();
			NewLine();
			Write("]");
		}
		else
		{
			Write("{}", Node.Value);
		}
	}

	void FArchive::Write(FStringView String)
	{
		Buffer += String;
		WriteHistoryStack.push_front(String.length());
	}

	void FArchive::NewLine()
	{
		FString Temp;

		Temp += "\n";

		for (size_t i = 0; i < PushedScopes; ++i)
		{
			Temp += "  ";
		}

		Write(Temp);
	}

	void FArchive::Undo()
	{
		const size_t LastWrittenCharacters = WriteHistoryStack.front();
		WriteHistoryStack.pop_front();

		for (size_t i = 0; i < LastWrittenCharacters; ++i)
		{
			Buffer.pop_back();
		}
	}

	void FArchive::PushScope()
	{
		++PushedScopes;
	}

	void FArchive::PopScope()
	{
		BS_ASSERT(PushedScopes, "PopScope() called when PushedScopes == 0");
		--PushedScopes;
	}
}
