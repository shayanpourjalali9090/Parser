#include <vector>
#include <string>
#include <iostream>

enum class token_type{
    NUMBER,
    OPERATOR,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,

};

std::string token_type_to_string(token_type type){
    switch (type){
        case token_type::CLOSE_PARENTHESIS:{return "CLOSE_PARENTHESIS";}
        case token_type::OPEN_PARENTHESIS:{return "OPEN_PARENTHESIS";}
        case token_type::NUMBER:{return "NUMBER";}
        case token_type::OPERATOR:{return "OPERATOR";}
        default: return "NO_IDEA";
    }

}



class char_buf{
    public:
    std::vector<char> buf;

    char_buf(){}

    void append(char c){
        buf.push_back(c);
    }

    std::string get(){
        std::string ret;
        for (char c:buf){
            ret+=c;
        }
        return ret;
    }

    bool is_empty(){
        return buf.empty();
    }

    void clear(){
        buf.clear();
    }

};

class token{
    public:

    token(){}

    token(token_type type, std::string value){
        this->type=type;
        this->value=value;
    }

    token(token_type type){
        this->type=type;
    }

    token_type type;
    std::string value;

};

class node{
    public:
    node(){}
    node(std::string value){
        this->value=value;
    }
    node *left=nullptr,*right=nullptr;
    std::string value;

};

char delimiters[]={'+','%','-','*','/','(',')',' ',};
char operators[]={'+','%','-','*','/',};

bool is_delimiter(char c){
    for (char delimiter:delimiters){
        if (delimiter==c)return true;
    }
    return false;
}

bool is_numeric(std::string str){
    for (char c:str){
        if (c<'0' || c>'9')return false;
    }
    return true;
}

bool is_operator(char c){
    for (char op:operators){
        if (op==c)return true;
    }
    return false;
}

std::vector<token> tokens;
int token_index=0;

int lex(std::string expression){
    char_buf buf;
    
    for (int i=0;i<(int)expression.size();i++){
        char cur=expression.at(i);

        if (!is_delimiter(cur)){
            buf.append(cur);
        }
        else{
            std::string str=buf.get();

            token t;
            t.type=token_type::NUMBER;
            t.value=str;

            buf.clear();
            if (!str.empty()){
                if (!is_numeric(str))return 1;

                tokens.push_back(t);
            }

            token t2;

            if (is_operator(cur)){
                t2.type=token_type::OPERATOR;
                t2.value={cur};

            }
            else if (cur=='('){
                t2.type=token_type::OPEN_PARENTHESIS;

            }
            else if (cur==')'){
                t2.type=token_type::CLOSE_PARENTHESIS;

            }
            else if (cur==' '){
                continue;
            }

            tokens.push_back(t2);

        }

    }
    if (!buf.is_empty()){
        std::string str=buf.get();
        
        token t;
        t.type=token_type::NUMBER;
        t.value=str;
        
        buf.clear();

        if (!is_numeric(str))return 1;

        tokens.push_back(t);


    }
    return 0;

}

token advance(){
    token ret=tokens.at(token_index++);
    return ret;
}

token peek(){
    return tokens.at(token_index);
}

bool at_end(){
    return token_index==tokens.size();
}

bool expect(std::string expected){
    std::string actual=advance().value;
    return actual==expected;
}

bool expect(token_type expected){
    token_type actual=advance().type;
    
    return actual==expected;

}

bool match(std::string c){
    return peek().value==c;
}

node* parse_expression();

node* parse_factor(){ // NUMBER | ( EXPRESSION )
    if (peek().type==token_type::NUMBER){
        return new node(advance().value);

    }
    else if (peek().type==token_type::OPEN_PARENTHESIS){
        advance();
        node* ret=parse_expression();
        bool e1=expect(token_type::CLOSE_PARENTHESIS);
        return !e1?nullptr:ret;

    }
    else {
        return nullptr;
    }

}

node* parse_term(){ // '*' | '/' | '%'
    node* n=parse_factor();

    while (!at_end() && (match("/") || match("%") || match("*"))){
        node* new_node=new node();

        new_node->value=peek().value;
        advance();

        new_node->left=n;
        new_node->right=parse_factor();

        n=new_node;
        

    }

    return n;

}

node* parse_expression(){ // '+' | '-'
    node* n=parse_term();

    while (!at_end() && (match("+") || match("-"))){
        node* new_node=new node();
        new_node->value=peek().value;
        advance();

        new_node->left=n;
        new_node->right=parse_term();

        n=new_node;
        
    }

    return n;
}

long long walk_tree(node* node){
    long long ans;
    
    if (node->value=="+"){
        ans=walk_tree(node->left)+walk_tree(node->right);
    }
    else if (node->value=="-"){
        ans=walk_tree(node->left)-walk_tree(node->right);

    }
    else if (node->value=="*"){
        ans=walk_tree(node->left)*walk_tree(node->right);
    }
    else if (node->value=="%"){
        ans=walk_tree(node->left)%walk_tree(node->right);
    }
    else if (node->value=="/"){
        ans=walk_tree(node->left)/walk_tree(node->right);
    }
    else{
        ans=std::stoll(node->value);
    }

    return ans;
}

int main(int argc, char** argv){
    if (argc<2){
        std::cerr << "No expression given" << std::endl;
        return 1;
    }

    std::string expression=argv[1];

    int result=lex(expression);
    if (result){
        std::cerr << "Cannot lex expression: '" << expression << "'" << std::endl;
        return result;
    }
    
    node* tree=parse_expression();

    std::cout << walk_tree(tree) << std::endl;
    
    return 0;
}