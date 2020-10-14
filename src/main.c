#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define DEBUG_                                  \
    printf("********************\n");           \
    printf("Function name: %s\n", __func__);    \
    printf("Line: %d\n", __LINE__);             \
    printf("********************\n");


/* global variables */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

int callcount = 0;
int returncount = 0;

struct tokenpair{
    char token_type[30];
    char token_string[100];
};
struct tokenpair prog_lines[200];

//Lexical Analysis prototypes
int lookup(char);
void addChar();
void getChar();
void getNonBlank();
int lex();

//Syntal Analysis prototypes
void LLparser();
void program();
void statements();
void stmt();
void expr();
void term();
void termtail();
void factor();
void factortail();
void consume();

void error();


/* Token classes (Character classes) */
#define LETTER       0
#define DIGIT        1
#define ASSIGN       2
#define UNKNOWN     99

/* Token codes */
#define INT_LIT     10      // 상수
#define IDENT       11      // 식별자(변수명)
#define ASSIGN_OP   20      // :=
#define EQ_SIGN     21      // =
#define ADD_OP      22      // +
#define SUB_OP      23      // -
#define MULT_OP     24      // *
#define DIV_OP      25      // /
#define LEFT_PAREN  26      // (
#define RIGHT_PAREN 27      // )
#define COLON       28      // :
#define SEMI_COLON  29      // ;
#define UN_KNOWN    30      // 모르는것.

/*
Lex
 = 예약어와 ID는 같이 인식됨 (이후 lookup()에서 키워드인지/lookup인지 핀단. )
 = 심볼테이블 만듬 (유저가 정의한 이름, 속성등을 저장 e.g. <int> <name>)
 - getchar() => 들어온 스트링의 다음 char받기, nextChar 변수에 넣기, charClass에 속하는지 보기
 - addchar() => nextChar 변수에 있던 값을 스트링 배열 lexeme으로 넘기기
 - getNonBlank() => ws/주석 제외하기
 - lookup() => +, *, ( 같은 단일문자 토큰들의 토큰코드 반환, lexeme이 지닌 string이 예약어인지 판단.
*/

int main(int argc, char* argv[]){
    //입력 파일을 열고 그 안의 내용물(텍스트)들을 처리함
    if( !strcmp(argv[1], "-v") ){
        //do verbose!!
        printf("-v 들어옴!\n");
        //select tokentype() 통해서 #define 된 숫자들 -> "<token name, 인식된 토큰>"
    }

    if( (in_fp = fopen(argv[1], "r") ) == NULL){
        printf("ERROR while Opening file.\n");
        return 1;
    }else{
        printf("File Opened.\n");
        getChar();
        do{
            // printf("call lex..\n");
            lex();
            printf("call initial LLparser..\n");
            LLparser();
        }while (nextToken != EOF);
    }
    fclose(in_fp);
    printf("file closed.\n");
    return 0;
}

/*lookup - a function to lookup operators and parentheses and return the token*/
int lookup(char ch){
    switch (ch){
        case'=':    addChar();      nextToken = EQ_SIGN;        break;
        case'+':    addChar();      nextToken = ADD_OP;         break;
        case'-':    addChar();      nextToken = SUB_OP;         break;
        case'*':    addChar();      nextToken = MULT_OP;        break;
        case'/':    addChar();      nextToken = DIV_OP;         break;
        case'(':    addChar();      nextToken = LEFT_PAREN;     break;
        case')':    addChar();      nextToken = RIGHT_PAREN;    break;
        case':':    addChar();      nextToken = COLON;          break;
        case';':    addChar();      nextToken = SEMI_COLON;     break;
//        default:    addChar();      nextToken = EOF;            break;
        default:    addChar();      nextToken = UN_KNOWN;        break;
    }
//    printf("%d\n", nextToken);
    return nextToken;
}

/*addChar - a function to add nextChar to lexeme*/
void addChar(){
    if(lexLen <= 98){
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }else{
        printf("ERROR - lexeme is too long \n");
    }
}

/*getChar - a function to get the next character of input
                        and determine its character class*/
void getChar(){
    if( (nextChar = getc(in_fp)) != EOF )
    {
        if ( isalpha(nextChar) ){                        charClass = LETTER;
        //printf("isalpha called!!\n");
        }
        else if( isdigit(nextChar) ){                    charClass = DIGIT;
        //printf("isdigit called!!\n");
        }
        else if( nextChar == ':' || nextChar == '='){    charClass= ASSIGN;
        //printf(": or = is called!!\n");
        }
        else{                                            charClass = UNKNOWN;
        //printf("UNKNOWN called!!\n");
        }
    }
    else charClass = EOF;
}

/*getNonBlank - a function to call getChar until it returns
                                a non-whitespace character*/
void getNonBlank(){
    while( isspace(nextChar) ){
        //printf("getNonBlank()\n");
        getChar();
    }
}

/*lex - a simple lexical analyzer for arithmetic expressions*/
int lex(){
    ++callcount;
    ++returncount;/////
    printf("lex() callcount: %d\n", callcount);/////
    lexLen = 0;
    getNonBlank();
//    printf("%d\n", charClass);
    switch(charClass){
        /*Parse identifiers*/
        case LETTER:
            addChar();
            getChar();
            while(charClass == LETTER || charClass == DIGIT){
                addChar();
                getChar();
            }
            nextToken = IDENT;
            break;
        case DIGIT:
            addChar();
            getChar();
            while(charClass == DIGIT){
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;
        case ASSIGN:
            while(nextChar == ':' || nextChar == '=' ){   //=a같이 토큰화 되는 것 방지
                    addChar();
                    getChar();
                }
            nextToken = ASSIGN_OP;
            break;
        case UNKNOWN:
                lookup(nextChar);
                getChar();
            printf("no way!\n");
            break;
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    } /*End of switch*/
    printf("**Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    strcpy(prog_lines[callcount].token_string, lexeme);//////
//    printf("token_string copied");
    printf("lex() returncount: %d\n", returncount);/////
    return nextToken;
}

/*
PARSER
 =
 - lex는 다음 렉심을 받아서 nextToken변수에 토큰코드를 넣는다.
 - Nonterminal: 하나의 RHS만을 가진다.
 - RHS에 있는 각 터미널 심볼을 nextToken과 비교한다. 만약 매칭되면 lex()는 다음 인풋을 가지러간다.
   아니면, 신텍스에러를 만든다.
 - 화살표 오른쪽(RHS)의 각 Nonterminal은 연관된 파싱 서브프로그램들을 호출한다.
 - 만일 RHS에서 고를 수 있는 Nonterminal이 여러개라면, 어떤 RHS로 파싱할지 정한다.
   nextToken은 매칭되는게 찾아질 때 까지, 각 RHS들에서 생성될 수 있는 첫 토큰과 비교된다.
 - 만일 매칭되는게 그래도 없다면, 신텍스 에러를 만든다. 실제 파서에서는 진단메시지를 만든다.
   또한 고칠 수 있는 에러라면, 워닝메시지를 만들고 파싱을 계속 할 수 있게 복구한다.
*/

void LLparser(){
    printf("Enter <LLparser>\n");
    program();
}

void program(){
    printf("Enter <program>\n");
    statements();
}

void statements(){
    printf("Enter <statements>\n");
    stmt();
    if(nextToken == SEMI_COLON){
        printf("::nextToken == SEMI_COLON\n");
        consume();
        lex();
        statements();
    }
}

void stmt(){    //error handling 필요.
    printf("Enter <stmt>\n");
    if(nextToken == IDENT){
        printf("::nextToken == IDENT\n");
        //터미널 심볼이 matching되므로 토큰스트링에 넣기. (consume)
        //consume lexeme and call lex()
        consume();
        lex();
    }
    if(nextToken == ASSIGN_OP){
        printf("::nextToken == ASSIGN_OP\n");
        consume();
        lex();
    }
    expr();
}

void expr(){
    printf("Enter <expr>\n");
    term();
    termtail();
}

void term(){
    printf("Enter <term>\n");
    factor();
    factortail();
}

void termtail(){
    /* If the next token is + or -, call lex to get the next token and parse the next term*/
    printf("Enter <termtail>\n");
    while(nextToken == ADD_OP){
        printf("::nextToken == ADD_OP\n");
        consume();
        lex();
        term();
        termtail();
    }
}
void factor(){
    printf("Enter <factor>\n");
    if(nextToken == IDENT || nextToken == INT_LIT){
        printf("::nextToken == IDENT or INT_LIT\n");
        consume();
        lex();
    }else if(nextToken == LEFT_PAREN){
        printf("::nextToken == LEFT_PAREN\n");
        consume();
        lex();
        expr();
        if(nextToken == RIGHT_PAREN){
            printf("::nextToken == RIGHT_PAREN\n");
            consume();
            lex();
        }else{
            DEBUG_
            error(); //LEFT_PAREN도, RIGHT_PAREN도 아니었다.
        }
    }else{
        DEBUG_
        error(); //ID도, INT_LIT도, LEFT_PAREN도 아니었다.
    }
}

void factortail(){
    printf("Enter <factortail>\n");
    if(nextToken == MULT_OP){
        while(nextToken == MULT_OP){
            consume();
            lex();
            factor();
            factortail();
        }
    }else if(nextToken >= 0){        //Epsilon, nextToken에 뭔가있는데 에러/워닝은아닌거
        return;
    }else{
        DEBUG_
        error();
    }
}

void error(){
    printf("*ERROR/WARNING while parsing*\n");
}

void consume(){
    switch(nextToken)
    {
    case ASSIGN:          strcpy(prog_lines[callcount].token_type, "OP_assign");     break;
    case INT_LIT:         strcpy(prog_lines[callcount].token_type, "Const");         break;
    case IDENT:           strcpy(prog_lines[callcount].token_type, "ID");            break;
    case EQ_SIGN:         strcpy(prog_lines[callcount].token_type, "UnKnown");       break;
    case ADD_OP:          strcpy(prog_lines[callcount].token_type, "OP_add");        break;
    case SUB_OP:          strcpy(prog_lines[callcount].token_type, "OP_sub");        break;
    case MULT_OP:         strcpy(prog_lines[callcount].token_type, "OP_mult");       break;
    case DIV_OP:          strcpy(prog_lines[callcount].token_type, "OP_div");        break;
    case LEFT_PAREN:      strcpy(prog_lines[callcount].token_type, "L_paren");       break;
    case RIGHT_PAREN:     strcpy(prog_lines[callcount].token_type, "R_paren");       break;
    case COLON:           strcpy(prog_lines[callcount].token_type, "UnKnown");       break;
    case SEMI_COLON:      strcpy(prog_lines[callcount].token_type, "UnKnown");       break;
    default:              strcpy(prog_lines[callcount].token_type, "UnKnown");       break;
    }
    printf("token_type copied.. \n");
}
