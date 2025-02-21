#include "Misc/AssertionMacros.h"
#include "Serialization/Archive.h"

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
		TArray<FToken> Tokenize(FStringView Source)
		{
			this->Source = Source;
			Position = 0;
			TArray<FToken> Result;

			while (Position < Source.GetSize())
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

					while (Position < Source.GetSize() && (std::isdigit(Get()) || Get() == '.' || Get() == '-'))
					{
						TokenValue += Consume();
					}
				}
				else if (CurrentChar == '"')
				{
					TokenValue += Consume();
					TokenType = ETokenType::StringLiteral;

					while (Position < Source.GetSize() && Get() != '"')
					{
						TokenValue += Consume();
					}

					if (Position < Source.GetSize())
					{
						TokenValue += Consume();
					}
				}

				Result.EmplaceBack(std::move(Token));
			}

			return Result;
		}

	private:
		FStringView Source;
		size_t Position = 0;

		char Get() const
		{
			return Source[Position];
		}

		char Consume()
		{
			return Source[Position++];
		}

		char Peek() const
		{
			return (Position + 1 < Source.GetSize()) ? Source[Position + 1] : '\0';
		}
	};

	class FParser final
	{
	public:
		TUnorderedMap<FString, FEncodedNode> Parse(const TArray<FToken>& Tokens)
		{
			this->Tokens = &Tokens;

			TUnorderedMap<FString, FEncodedNode> Result;

			while (TokenIndex < Tokens.GetSize())
			{
				if (Get().Type == ETokenType::StringLiteral)
				{
					const FToken& Token = Consume();
					const FString& Value = Token.Value;
					FString UnquotedValue = FString(Value.GetData() + 1, Value.GetSize() - 2);
					FEncodedNode& NewNode = Result[std::move(UnquotedValue)];

					if (Consume().Value != ":")
					{
						BS_LOG(Fatal, "Failed to parse data: Token \":\" is required after {} in top level", Token.Value);
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
						BS_LOG(Fatal, "Failed to parse data: Expected after token \":\": string literal or number literal or sequence begin");
					}
				}
			}

			return Result;
		}

	private:

		// BSTODO: Replace with TArrayView

		const TArray<FToken>* Tokens;
		size_t TokenIndex = 0;

		TArray<FEncodedNode> ParseSequence()
		{
			++TokenIndex;

			TArray<FEncodedNode> Result;

			while (Get().Value != "]")
			{
				if (Get().Value == "[")
				{
					Result.EmplaceBackGetRef().Children = ParseSequence();
				}
				else if (Get().Type == ETokenType::StringLiteral)
				{
					Result.EmplaceBackGetRef().Value = Consume().Value;
				}
				else if (Get().Type == ETokenType::NumberLiteral)
				{
					Result.EmplaceBackGetRef().Value = Consume().Value;
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
			return (*Tokens)[TokenIndex];
		}

		const FToken& Consume()
		{
			return (*Tokens)[TokenIndex++];
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

		const FString& Content = StringStream.str().data();
		const TArray<FToken>& Tokens = FTokenizer().Tokenize(Content);

		NodeTree = FParser().Parse(Tokens);
	}

	void FArchive::Flush()
	{
		Stream.close();
		Stream.open(FilePath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		for (const TPair<const FString, FEncodedNode> NodePair : NodeTree)
		{
			const FString& Name = NodePair.first;
			const FEncodedNode& Node = NodePair.second;
			WriteFmt("\"{}\": ", Name);
			WriteNodeDataRecursive(Node);
			NewLine();
		}
		Stream << Buffer.GetData();
		Buffer.Clear();
		bNeedFlush = false;
	}

	void FArchive::WriteNodeDataRecursive(const FEncodedNode& Node)
	{
		// BSTODO: Replace recursion with loop
		const TArray<FEncodedNode>& Children = Node.Children;
		const size_t ChildrenCount = Children.GetSize();
		if (ChildrenCount)
		{
			Write("[");
			PushIndent();
			NewLine();
			for (size_t i = 0; i < ChildrenCount - 1; ++i)
			{
				WriteNodeDataRecursive(Children[i]);
				Write(",");
				NewLine();
			}
			WriteNodeDataRecursive(Children[ChildrenCount - 1]);
			PopIndent();
			NewLine();
			Write("]");
		}
		else
		{
			WriteFmt("{}", Node.Value);
		}
	}
}
