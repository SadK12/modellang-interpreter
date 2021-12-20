#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>
#include <map>

using namespace std;

enum type_of_lex {
    LEX_NULL,                                                                                                       /* 0*/
    LEX_AND, LEX_BOOL, LEX_BREAK, LEX_DO, LEX_ELSE, LEX_FALSE, LEX_FOR, LEX_GOTO, LEX_IF, LEX_INT,                  /*11*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_STRING, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE,                    /*19*/
    LEX_FIN,                                                                                                        /*20*/
    LEX_SEMICOLON, LEX_COMMA, LEX_QUOTE, LEX_COLON, LEX_COMP, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,              /*29*/
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_MOD, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_LBRACE, LEX_RBRACE, /*40*/
    LEX_NUM,                                                                                                        /*41*/
    LEX_STR,                                                                                                        /*42*/
    LEX_ID,                                                                                                         /*43*/
    POLIZ_LABEL,                                                                                                    /*44*/
    POLIZ_ADDRESS,                                                                                                  /*45*/
    POLIZ_GO,                                                                                                       /*46*/
    POLIZ_FGO                                                                                                       /*47*/
//    POLIZ_SEMICOLON
};
class Lex {
    type_of_lex t_lex;
    int v_lex;
    string strv_lex;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0, string strv = ""): t_lex(t), v_lex(v), strv_lex(strv) {}
    type_of_lex  get_type() const { return t_lex; }
    int get_value() const { return v_lex; }
    string get_strvalue() const { return strv_lex; }
    friend ostream& operator<< (ostream &s, Lex l);
    Lex& operator= (const Lex& a);
};
Lex& Lex::operator= (const Lex& a) {
    this->t_lex = a.t_lex;
    this->v_lex = a.v_lex;
    this->strv_lex = a.strv_lex;
    return *this;
}
class Ident {
    string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
    string strvalue;
public:
    Ident();
    Ident(const string n);
    bool operator==(const string& s) const { return name == s; }
    string get_name() const { return name; }
    bool get_declare() const { return declare; }
    void set_declare() { declare = true; }
    type_of_lex get_type() const { return type; }
    void set_type(type_of_lex t) { type = t; }
    bool get_assign() const { return assign; }
    void set_assign() { assign = true; }
    int  get_value() const { return value; }
    void set_value(int v) { value = v; }    
    string  get_strvalue() const { return strvalue; }
    void set_strvalue(string v) { strvalue = v; }
};
Ident::Ident() {
    declare = false; 
    assign = false; 
}
Ident::Ident(const string n) {
    name = n;
    declare = false; 
    assign = false;
}

vector<Ident> TID;
 
int put(const string& buf) {
    vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end()) {
        return (k - TID.begin());    
    }
    TID.push_back(Ident(buf));
    return (TID.size() - 1);
}

class Scanner {
    FILE* fp;
    char c;
    int look(const string buf, const char** list);
    void gc() { c = fgetc(fp); }
public:
    static const char* TW[];
    static const char* TD[];
    Scanner(const char* program) {
        if (!(fp = fopen (program, "r"))) 
            throw "can’t open file";
    }
    Lex get_lex();
};
int Scanner::look(const string buf, const char** list) {
    int i = 0;
    while (list[i]) {
        if (buf == list[i]) {
            return i;    
        }
        ++i;
    }
    return 0; 
}

const char* Scanner::TW[] = { "", "and", "bool", "break", "do", "else", "false", "for", "goto", "if", "int", "not", "or",
                            "program", "read", "string", "true", "var", "while", "write", NULL };
 
const char* Scanner::TD[] = { "@", ";", ",", "\"", ":", "==", "(", ")", 
                                "=", "<", ">", "+", "-", "*", "/", "%", "<=", "!=", ">=", "{", "}", NULL };
 
Lex Scanner::get_lex() {
    enum state {H, IDENT, NUMB, STR, SLASH, COM, ALE, NEQ};
    int d, j;
    string buf;
    state CS = H;
    do {
        gc();
        switch (CS) {
            case H:
                if (c == ' ' || c == '\n' || c== '\r' || c == '\t')  {} 
                else if (isalpha(c)) {
                    buf.push_back(c);
                    CS = IDENT;
                }
                else if (isdigit(c)) {
                    d = c - '0';
                    CS = NUMB;
                } 
                else if (c == '\"') {
                    CS = STR;
                }
                else if (c == '/') {
                    CS = SLASH;
                }
                else if (c == '=' || c == '<' || c == '>') { 
                    buf.push_back(c);
                    CS = ALE; 
                }
                else if (c == '@')
                    return Lex(LEX_FIN);
                else if (c == '!') {
                    buf.push_back(c);
                    CS = NEQ;
                }
                else {
                    buf.push_back(c);
                    if ( (j = look(buf, TD))) {
                        return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                    }
                    else { throw c; }
                }
                break;
            case IDENT:
                if (isalpha(c) || isdigit(c)) {
                    buf.push_back(c); 
                }
                else {
                    ungetc(c, fp);
                    if ((j = look(buf, TW))) {
                        return Lex((type_of_lex)j, j);
                    } else {
                        j = put(buf);
                        return Lex(LEX_ID, j);
                    }
                    CS = H;
                }
                break;
            case NUMB:
                if (isdigit(c)) {
                    d = d*10 + (c - '0');
                } else {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, d);
                }
                break;
            case STR:
                if (c == '\n') {throw "undef str\n";}
                if (c != '\"') {
                    if (c == '\\') {
                        gc();
                        if (c == '\"') {
                            buf.push_back(c); 
                        } else {
                            buf.push_back('\\');
                            buf.push_back(c);
                        }
                    } else {
                        buf.push_back(c); 
                    }
                } else {
                    return Lex(LEX_STR, 0, buf);
                    CS = H;
                }
                break;
            case SLASH:
                if (c == '*') {
                    CS = COM;
                } else {
                    ungetc(c, fp);
                    j = look("/", TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;
            case COM:
                if (c == EOF) {throw "undef comment\n";}
                if (c == '*') {
                    gc();
                    if (c == '/') {
                        CS = H;
                    } else {
                        ungetc(c, fp);
                    }
                } else if (c == '@')
                    throw c;
                break;
            case ALE:
                if (c == '=') {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                } else {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;
            case NEQ:
                if (c == '=') {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex(LEX_NEQ, j);
                } else
                    throw '!';
                break;
    } //end switch
  } while (true);
}
 
ostream & operator<< (ostream &s, Lex l) {
    string t;
    if (l.t_lex <= LEX_WRITE) { t = Scanner::TW[l.t_lex]; }
    else if (l.t_lex >= LEX_FIN && l.t_lex <= LEX_RBRACE) { t = Scanner::TD[ l.t_lex - LEX_FIN ]; }
    else {
        switch (l.t_lex) {
            case LEX_NUM: 
                t = "NUMB";
                break;
            case LEX_STR:
                t = "STR";
                return s << "< " << t << ", " << l.strv_lex << " >;" << endl;
                break;
            case LEX_ID:  
                t = TID[l.v_lex].get_name ();
                break;
            case POLIZ_LABEL:
                t = "Label"; 
                break;
            case POLIZ_ADDRESS:
                t = "Addr";
                break;
            case POLIZ_GO:
                t = "!";
                break;
            case POLIZ_FGO: 
                t = "!F";
                break;
            default:
                throw l;
                break;
        }
    }
    s << "< " << t << ", " << l.v_lex << " >;" << endl;
    return s;
}

//////////////////////////  Класс Parser  /////////////////////////////////
template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) {
    i = st.top(); st.pop();
}
 
class Parser {
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    string cstr_val;
    type_of_lex temp_type;
    stack <Lex> buf_lex;
    stack <int> break_pl;
    Scanner scan;
    stack <int> st_int;
    stack <type_of_lex> st_lex;
    map <int, int> mark;
    map <int, vector<int> > goto_map;
    bool for_flag = false;
    bool while_flag = false;
    int assign_c = 0;
    void S();
    void A();
    void B();
    void O();
    void E();
    void I();
    void K();
    void V();
    void Z();
    void Z1();
    void W();
    void L();
    void N();
    void R();
    void Q();
    void T();
    void dec(type_of_lex type);
    void check_id();
    void check_op();
    void check_not();
    void check_minus();
    void check_incycle();
    void eq_type();
    void eq_bool();
    void check_id_in_read();
    void label_control();
    void gl();
public:
    vector <Lex> poliz;
    Parser (const char* program):scan(program) {}
    void analyze();
};
void Parser::gl() {
    if (buf_lex.empty()){
        curr_lex = scan.get_lex();
        c_type = (type_of_lex)curr_lex.get_type();
        c_val = curr_lex.get_value();
        cstr_val = curr_lex.get_strvalue();
    } else {
        Lex temp;
        from_st(buf_lex, temp);
        curr_lex = temp;
        c_type = (type_of_lex)temp.get_type();
        c_val = temp.get_value();
        cstr_val = temp.get_strvalue();
    }
//    cout << "Lex: " << c_type << ' ' << c_val << ' ' << cstr_val << endl;
}
void Parser::analyze() {
    gl();
    S();
}
void Parser::dec(type_of_lex type) {
    int i;
    while (!st_int.empty()) {
        from_st (st_int, i);
        if (TID[i].get_declare()) {
            throw "declared twice\n";
        }
        else {
            TID[i].set_declare();
            TID[i].set_type(type);
        }
    }
}
void Parser::check_id() {
    if (TID[c_val].get_declare()) {
        st_lex.push(TID[c_val].get_type());
    }
    else {
        throw "not declared";
    }
}
 
void Parser::check_op() {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
    
    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);
    
    if (op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH) {
        r = LEX_INT;
        t = LEX_INT;
    }
    if (op == LEX_PLUS) {
        if (t1 == LEX_STRING || t1 == LEX_INT) {
            t = t1;
            r = t1;
        } else {
            throw "wrong operands in +";
        }
    }
    if (op == LEX_OR || op == LEX_AND) {
        t = LEX_BOOL;
        r = LEX_BOOL;
    }
    if (op == LEX_EQ) {
        if (t1 != t2) {
            throw "wrong types are in operation ="; 
        }
        t = t1;
        r = t1;
        
    }
    if (op == LEX_COMP || op == LEX_NEQ || op == LEX_GEQ || op == LEX_LEQ || op == LEX_GTR || op == LEX_LSS) {
        t = t1;
        r = LEX_BOOL;
    }
    if (t1 == t2  &&  t1 == t) {
        st_lex.push(r);
    } 
    else { 
        throw "wrong types are in operation"; 
    }
    if (op == LEX_EQ) {
        assign_c++;
    } else {
        poliz.push_back(Lex(op));
    }
}
 
void Parser::check_not() {
    if (st_lex.top() != LEX_BOOL){
        throw "wrong type is in not";
    }
    else { 
        poliz.push_back(Lex(LEX_NOT));
    }
}
void Parser::check_minus() {
    if (st_lex.top() != LEX_INT){
        throw "wrong type is in unar.minus";
    }
    else { 
        poliz.push_back(Lex(LEX_MINUS));
    }
}
void Parser::check_incycle() {
    if (!(while_flag || for_flag)){
        throw "break not in cycle";
    }
    else { 
        poliz.push_back(Lex(LEX_NOT));
    }
}
void Parser::eq_type() {
    type_of_lex t;
    from_st (st_lex, t);
    if (t != st_lex.top()) { 
        throw "wrong types are in :=";
    }
    st_lex.pop();
}
 
void Parser::eq_bool() {
    if (st_lex.top() != LEX_BOOL) {
        throw "expression is not boolean";
    }
    st_lex.pop();
}
 
void Parser::check_id_in_read () {
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}
void Parser::label_control() {
    map <int, vector<int> >::iterator mIter = goto_map.begin();
    map <int, int>::iterator markIt = mark.begin();
    while (mIter != goto_map.end()) {
        if(!mark.count(mIter->first))  {
            throw "undefined mark";
        }
        mIter++;
    }
    mIter = goto_map.begin();
    while (mIter != goto_map.end()) {
        vector<int> temp_vect = mIter->second;
        vector<int>::size_type i = 0;
        while (i != temp_vect.size()) {
            int temp_pl = temp_vect[i];
            poliz[temp_pl] = Lex(POLIZ_LABEL, markIt->second);
            i++;
        }
        mIter++;
        markIt++;
    }
}
void Parser::S() {
    if (c_type == LEX_PROGRAM) {
        gl();
        if (c_type == LEX_LBRACE) {
            A();
            B();
            if (c_type != LEX_RBRACE) {
                throw curr_lex;
            }
            label_control();
        } else { throw curr_lex; }
    } else { throw curr_lex; }
}
void Parser::A() {
    gl();
    if (c_type == LEX_INT || c_type == LEX_STRING || c_type == LEX_BOOL) {
        temp_type = c_type;
        E();
        while (c_type == LEX_COMMA) {
            E();
        }
        if (c_type != LEX_SEMICOLON) {
            throw curr_lex;
        } else {
            dec(temp_type);
        }
        A();
    }
}
void Parser::E() {
    gl();
    if (c_type != LEX_ID) {
        throw curr_lex;
    } else {
        st_int.push(c_val);
        gl();
        if (c_type == LEX_EQ) {
            poliz.push_back(Lex(POLIZ_ADDRESS, st_int.top()));
            I();
        }
    }
}
void Parser::I() {
        gl();
        K();
        poliz.push_back(Lex(LEX_EQ));
//        poliz.push_back(Lex(POLIZ_SEMICOLON));
}
void Parser::K() {
    int temp_tid;
    if (c_type == LEX_PLUS || c_type == LEX_MINUS) {
        gl();
        if (c_type == LEX_NUM && temp_type == LEX_INT) {
            temp_tid = st_int.top();
            TID[temp_tid].set_assign();
            poliz.push_back(Lex(c_type, c_val, cstr_val));
            gl();
        } else { throw curr_lex; }
    } else if (c_type == LEX_NUM || c_type == LEX_TRUE || c_type == LEX_FALSE || c_type == LEX_STR) {
        switch (temp_type) {
            case LEX_INT: 
                if (c_type == LEX_NUM) {  
                    temp_tid = st_int.top();
                    TID[temp_tid].set_assign();
                    poliz.push_back(Lex(c_type, c_val, cstr_val));
                    gl();
                } else { throw "invalid assign"; }
                break;
            case LEX_STRING:
                if (c_type == LEX_STR) { 
                    temp_tid = st_int.top();
                    TID[temp_tid].set_assign();
                    poliz.push_back(Lex(c_type, c_val, cstr_val));
                    gl();
                } else { throw "invalid assign"; }
                break;
            case LEX_BOOL:
                if (c_type == LEX_TRUE || c_type == LEX_FALSE) { 
                    temp_tid = st_int.top();
                    TID[temp_tid].set_assign();
                    poliz.push_back(Lex(c_type, c_val, cstr_val));
                    gl();
                } else { throw "invalid assign"; }
                break;
        }
    } else {
        throw curr_lex;
    }
}
void Parser::B() {
    if (c_type == LEX_IF || c_type == LEX_FOR || c_type == LEX_WHILE || c_type == LEX_BREAK || c_type == LEX_GOTO || c_type == LEX_READ || c_type == LEX_WRITE || c_type == LEX_LBRACE || c_type == LEX_ID || c_type == LEX_NUM || c_type == LEX_STR || c_type == LEX_NOT || c_type == LEX_LPAREN || c_type == LEX_FALSE || c_type == LEX_TRUE)  {
        O();
        B();
    }
}
void Parser::O() {                  // процедура для блока операторов
    int pl0, pl1, pl2, pl3, pl4, pl5, pl6, pl7;
    
    if (c_type == LEX_IF) {
//        cout << "зашли в if" << endl;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            V();
            eq_bool();
            pl2 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            if (c_type == LEX_RPAREN) {
                gl();
                O();
                pl3 = poliz.size();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
//                cout << "вышли из if идем к else" << endl;
                if (c_type == LEX_ELSE) {
                    gl();
                    O();
                    poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
                } else { throw curr_lex; }
            } else { throw curr_lex; }
        } else { throw curr_lex; }
    } else if (c_type == LEX_FOR) {
        int pl10 = poliz.size();
        for_flag = true;
//        cout << "вошли в for" << endl;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            Z();
//            poliz.push_back(Lex(POLIZ_SEMICOLON));
//            cout << "отработало первое условие" << endl;
            if (c_type == LEX_SEMICOLON) {
                pl6 = poliz.size();
                gl();
                if (c_type != LEX_SEMICOLON) { Z1(); }
                pl4 = poliz.size();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_FGO));
                pl5 = poliz.size();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                pl7 = poliz.size();
//                cout << "отработало второе условие" << endl;
                if (c_type == LEX_SEMICOLON) {
                    gl();
                    Z();
                    poliz.push_back(Lex(POLIZ_LABEL, pl6));
                    poliz.push_back(Lex(POLIZ_GO));
//                    cout << "отработало третье условие" << endl;
                    if (c_type == LEX_RPAREN) {
                        gl();
                        poliz[pl5] = Lex(POLIZ_LABEL, poliz.size());
                        O();
                        poliz.push_back(Lex(POLIZ_LABEL, pl7));
                        poliz.push_back(Lex(POLIZ_GO));
                        poliz[pl4] = Lex(POLIZ_LABEL, poliz.size());
                        for_flag = false;
                        int temp_pl;
                        while (!break_pl.empty()) {
                            from_st(break_pl, temp_pl);
                            poliz[temp_pl] = Lex(POLIZ_LABEL, poliz.size());
                        }
                    } else { throw curr_lex; for_flag = false; }
                } else { throw curr_lex; for_flag = false; }
            } else { throw curr_lex; for_flag = false; }
        } else { throw curr_lex; for_flag = false; }
    } else if(c_type == LEX_WHILE) {
        while_flag = true;
//        cout << "вошли в while" << endl;
        gl();
        if (c_type == LEX_LPAREN) {
            pl0 = poliz.size();
            gl();
            V();
            eq_bool();
            pl1 = poliz.size(); 
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            if (c_type == LEX_RPAREN) {
                gl();
                O();
                poliz.push_back(Lex(POLIZ_LABEL, pl0));
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
                while_flag = false;
                int temp_pl;
                while (!break_pl.empty()) {
                    from_st(break_pl, temp_pl);
                    poliz[temp_pl] = Lex(POLIZ_LABEL, poliz.size());
                }
            } else { throw curr_lex; while_flag = false; }
        } else { throw curr_lex; while_flag = false; }
    } else if (c_type == LEX_BREAK) {
//        cout << "вошли в break;" << endl;
        gl();
        if (c_type == LEX_SEMICOLON) {
            gl(); 
            check_incycle();
            break_pl.push(poliz.size());
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
        } else { throw curr_lex; }
    } else if (c_type == LEX_GOTO) {
//        cout << "вошли в goto" << endl;
        gl();
        if (c_type == LEX_ID) {
            if (!goto_map.count(c_val)) {
                goto_map[c_val] = vector<int>(0);
                goto_map[c_val].push_back(poliz.size());
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
            } else {
                goto_map[c_val].push_back(poliz.size());
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
            }
            gl();
            if (c_type == LEX_SEMICOLON) {
                gl();
            } else { throw curr_lex; }
        } else { throw curr_lex; }
    } else if (c_type == LEX_READ) {
//        cout << "вошли в read" << endl;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            if(c_type == LEX_ID) {
                check_id_in_read();
                poliz.push_back(Lex(POLIZ_ADDRESS, c_val, cstr_val));
                gl();
                if (c_type == LEX_RPAREN) {
                    gl();
                    poliz.push_back(Lex(LEX_READ));
                    if (c_type == LEX_SEMICOLON) {
                        gl();
                    } else { throw curr_lex; }
                } else { throw curr_lex; }
            } else { throw curr_lex; }
        } else { throw curr_lex; }
    } else if (c_type == LEX_WRITE) {
//        cout << "вошли в write" << endl;
        int w_args = 0;
        gl();
        if (c_type == LEX_LPAREN) {
            gl();
            V();
            w_args++;
            while (c_type == LEX_COMMA) {
                gl();
                V();
                w_args++;
            }
            if (c_type == LEX_RPAREN) {
                gl();
                poliz.push_back(Lex(LEX_WRITE, w_args));
//                cout << "args in write: " << w_args << endl;
                if (c_type == LEX_SEMICOLON) {
                    gl();
                } else { throw curr_lex; }
            } else { throw curr_lex; }
        } else { throw curr_lex; }
    } else if (c_type == LEX_LBRACE) {
        gl();
        B();
        if (c_type == LEX_RBRACE) {
            gl();
        } else { throw curr_lex; }
    } else if (c_type == LEX_NUM || c_type ==  LEX_STR || c_type == LEX_NOT || c_type == LEX_LPAREN || c_type == LEX_FALSE || c_type == LEX_TRUE) { 
        V();
//        poliz.push_back(Lex(POLIZ_SEMICOLON));
        if (c_type == LEX_SEMICOLON) {
            gl();
        } else { throw curr_lex; }
    } else if (c_type == LEX_ID) {
        Lex temp_lex = curr_lex;
        int temp_val = c_val;
        gl();
        if (c_type == LEX_COLON) {
            if (!mark.count(temp_val)) {
                mark[temp_val] = poliz.size();
            } else { throw "double mark declaration"; }
            gl();
            O();
        } else if (c_type == LEX_EQ || c_type == LEX_OR || c_type == LEX_AND || c_type == LEX_COMP || c_type == LEX_NEQ || c_type == LEX_GEQ || c_type == LEX_LEQ || c_type == LEX_GTR || c_type == LEX_LSS || c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_TIMES || c_type == LEX_SLASH) {
            buf_lex.push(curr_lex);
            curr_lex = temp_lex;
            c_type = temp_lex.get_type();
            c_val = temp_lex.get_value();
            cstr_val = temp_lex.get_strvalue();
            V();
//            poliz.push_back(Lex(POLIZ_SEMICOLON));
            if (c_type == LEX_SEMICOLON) {
                gl();
            } else { throw curr_lex; }
        } else { throw curr_lex; }
    } else { 
        cout << "func O() takes invalid value" << endl;
        throw curr_lex; 
    }
} 
void Parser::V() {                      // процедура для выражения
    W();
    while (c_type == LEX_EQ) {
        st_lex.push(c_type);
        gl();
        Lex temp_l = poliz.back();
        poliz.pop_back();
        poliz.push_back(Lex(POLIZ_ADDRESS, temp_l.get_value(), temp_l.get_strvalue()));
        W();
        check_op();
    }
    while (assign_c > 0) {
        poliz.push_back(Lex(LEX_EQ));
        assign_c--;
    }
}
void Parser::W() {
    L();
    while (c_type == LEX_OR) {
        st_lex.push(c_type);
        gl();
        L();
        check_op();
    }
}
void Parser::L() {
    N();
    while (c_type == LEX_AND) {
        st_lex.push(c_type);
        gl();
        N();
        check_op();
    }
}
void Parser::N() {
    R();
    while (c_type == LEX_COMP || c_type == LEX_NEQ || c_type == LEX_GEQ || c_type == LEX_LEQ || c_type == LEX_GTR || c_type == LEX_LSS) {
        st_lex.push(c_type);
        gl();
        R();
        check_op();
    }
}
void Parser::R() {
    Q();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS) {
        st_lex.push(c_type);
        gl();
        Q();
        check_op();
    }
}
void Parser::Q() {
    T();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH) {
        st_lex.push(c_type);
        gl();
        T();
        check_op();
    }
}
void Parser::T() {
    if (c_type == LEX_NOT) {
        gl();
        T();
        check_not();
    } else if (c_type == LEX_MINUS) {
        poliz.push_back(Lex(LEX_NUM, 0));
        gl();
        T();
        check_minus();
    } else if (c_type == LEX_LPAREN) {
        gl();
        V();
        if (c_type == LEX_RPAREN) {
            gl();
        } else { throw curr_lex; }
    } else if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val, cstr_val));
        gl();
    } else if (c_type == LEX_STR) {
        st_lex.push(LEX_STRING);
        poliz.push_back(Lex(c_type, c_val, cstr_val));
        gl();
    } else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        poliz.push_back(Lex(c_type, c_val, cstr_val));
        gl();
    } else if (c_type == LEX_FALSE || c_type == LEX_TRUE) {
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(c_type, c_val, cstr_val));
        gl();
    }
}
void Parser::Z() {
    if (c_type == LEX_ID || c_type == LEX_NUM || c_type ==  LEX_STR || c_type == LEX_NOT || c_type == LEX_LPAREN || c_type == LEX_FALSE || c_type == LEX_TRUE) {
        V();
    } 
}
void Parser::Z1() {
    if (c_type == LEX_ID || c_type == LEX_NUM || c_type ==  LEX_STR || c_type == LEX_NOT || c_type == LEX_LPAREN || c_type == LEX_FALSE || c_type == LEX_TRUE) {
        V();
        eq_bool();
    } else { throw curr_lex; }
}

class Executer {
public:
    void execute (vector<Lex> & poliz);
};
void Executer::execute (vector<Lex>& poliz) {
    Lex pc_el;
    stack <Lex> args;
    stack <Lex> write_st;
    Lex l1, l2;
    int i, index = 0, size = poliz.size();
    while (index < size){
        pc_el = poliz[index];
        switch (pc_el.get_type()){
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
                args.push(Lex(LEX_INT, pc_el.get_value()));
                break;

            case LEX_STR:
                args.push(Lex(LEX_STRING, 0, pc_el.get_strvalue())); //todo
                break;

            case LEX_ID: 
                i = pc_el.get_value(); //todo
                if (TID[i].get_assign()){
                    if (TID[i].get_type() == LEX_INT || TID[i].get_type() == LEX_BOOL){
                        args.push(Lex(LEX_INT, TID[i].get_value()));
                    }
                    else if (TID[i].get_type() == LEX_STRING){
                        args.push(Lex(LEX_STRING, 0, TID[i].get_strvalue()));
                    }
                    else {
                        throw "POLIZ: wrong type of identificator";
                    }
                }
                else {
                  throw "POLIZ: indefinite identifier";
                }
                break;
 
            case LEX_NOT:
                from_st(args, l1);
                args.push(Lex(LEX_INT, static_cast<int>(!l1.get_value())));
                break;
 
            case LEX_OR:
                from_st(args, l1); 
                from_st(args, l2);
                i = static_cast<int>(l1.get_value() || l2.get_value());
                args.push(Lex(LEX_INT, i));
                break;
 
            case LEX_AND:
                from_st(args, l1);
                from_st(args, l2);
                i = static_cast<int>(l1.get_value() && l2.get_value());
                args.push(Lex(LEX_INT, i));
                break;
 
            case POLIZ_GO:
                from_st(args, l1);
                index = l1.get_value() - 1;
                break;
 
            case POLIZ_FGO:
                from_st(args, l1);
                from_st(args, l2);
                if (!l2.get_value()){
                    index = l1.get_value() - 1;
                }
                break;
 
            case LEX_WRITE: 
                i = pc_el.get_value();
                while (i > 0){
                    from_st(args, l1);
                    write_st.push(l1);
                    i--;
                }
                i = pc_el.get_value();
                while (i > 0){
                    from_st(write_st, l1);
                    if (l1.get_type() == LEX_INT){
                        cout << l1.get_value();
                    } else {
                        cout << l1.get_strvalue();
                    }
                    i--;
                }
                cout << endl;
                break;

            case LEX_READ:
                int k;
                from_st(args, l1);
                if (TID[l1.get_value()].get_type() == LEX_INT){
                    cout << "Input int value for " << TID[l1.get_value()].get_name() << endl;
                    cin >> k;
                    TID[l1.get_value()].set_value(k);
                }
                else if (TID[l1.get_value()].get_type() == LEX_BOOL){
                    string j;
                    while(1){
                        cout << "Input boolean value (true or false) for " << TID[l1.get_value()].get_name() << endl;
                        cin >> j;
                        if (j != "true" && j != "false"){
                            cout << "Error in input:true/false" << endl;
                            continue;
                        }
                        k = (j == "true") ? 1 : 0;
                        break;
                    }
                    TID[l1.get_value()].set_value(k);
                } else if (TID[l1.get_value()].get_type() == LEX_STRING){
                    string s;
                    cout << "Input string value for " << TID[l1.get_value()].get_name() << endl;
                    cin >> s;
                    TID[l1.get_value()].set_strvalue(s);
                }
                TID[l1.get_value()].set_assign();
                break;
 
            case LEX_PLUS:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = l2.get_value() + l1.get_value();
                    args.push(Lex(LEX_INT, k));
                } else {
                    string s = l2.get_strvalue() + l1.get_strvalue();
                    args.push(Lex(LEX_STRING, 0, s));
                }
                break;
 
            case LEX_TIMES:
                from_st(args, l1);
                from_st(args, l2);
                i = l2.get_value() * l1.get_value();
                args.push(Lex(LEX_INT, i));
                break;
 
            case LEX_MINUS:
                from_st(args, l1);
                from_st(args, l2);
                i = l2.get_value() - l1.get_value();
                args.push(Lex(LEX_INT, i));
                break;
 
            case LEX_SLASH:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_value()) {
                    i = l2.get_value() / l1.get_value();
                    args.push(Lex(LEX_INT, i));
                    break;
                }
                else
                    throw "POLIZ:divide by zero";
 
            case LEX_COMP:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = static_cast<int>(l1.get_value() == l2.get_value());
                    args.push(Lex(LEX_INT, k));
                } else {
                    int k = static_cast<int>(l1.get_strvalue() == l2.get_strvalue());
                    args.push(Lex(LEX_INT, k));
                }
                break;
 
            case LEX_LSS:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = static_cast<int>(l2.get_value() < l1.get_value());
                    args.push(Lex(LEX_INT, k));
                } else {
                    int k = static_cast<int>(l2.get_strvalue() < l1.get_strvalue());
                    args.push(Lex(LEX_INT, k));
                }
                break;
 
            case LEX_GTR:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = static_cast<int>(l2.get_value() > l1.get_value());
                    args.push(Lex(LEX_INT, k));
                } else {
                    int k = static_cast<int>(l2.get_strvalue() > l1.get_strvalue());
                    args.push(Lex(LEX_INT, k));
                }
                break;

            case LEX_LEQ:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = static_cast<int>(l2.get_value() <= l1.get_value());
                    args.push(Lex(LEX_INT, k));
                } else {
                    int k = static_cast<int>(l2.get_strvalue() <= l1.get_strvalue());
                    args.push(Lex(LEX_INT, k));
                }
                break;
 
            case LEX_GEQ:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = static_cast<int>(l2.get_value() >= l1.get_value());
                    args.push(Lex(LEX_INT, k));
                } else {
                    int k = static_cast<int>(l2.get_strvalue() >= l1.get_strvalue());
                    args.push(Lex(LEX_INT, k));
                }
                break;
 
            case LEX_NEQ:
                from_st(args, l1);
                from_st(args, l2);
                if (l1.get_type() == LEX_INT){
                    int k = static_cast<int>(l2.get_value() != l1.get_value());
                    args.push(Lex(LEX_INT, k));
                } else {
                    int k = static_cast<int>(l2.get_strvalue() != l1.get_strvalue());
                    args.push(Lex(LEX_INT, k));
                }
                break;
 
            case LEX_EQ:
                i = 0;
                do {
                    i++;
                    index++;
                } while (poliz[index].get_type() == LEX_EQ);
                index--;
                while (i > 0){
                    from_st(args, l1);
                    from_st(args, l2);
                    if (l1.get_type() == LEX_INT){
                        TID[l2.get_value()].set_value(l1.get_value());
                        TID[l2.get_value()].set_assign();
                        args.push(Lex(LEX_INT, l1.get_value()));
                    } else {
                        TID[l2.get_value()].set_strvalue(l1.get_strvalue());
                        TID[l2.get_value()].set_assign();
                        args.push(Lex(LEX_STRING, 0, l1.get_strvalue()));
                    }
                    i--;
                }
                break;

            default:
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };//end of while
    cout << endl << "Finish of executing!!!" << endl;
}

 
class Interpretator {
    Parser pars;
    Executer E;
public:
    Interpretator (const char* program): pars (program) {}
    void interpretation ();
};
 
void Interpretator::interpretation () {
    pars.analyze ();
    E.execute ( pars.poliz );
}

int main() { try {
        Interpretator I("prog.txt");
        cout << "Process of interpretation" << endl;
        I.interpretation();
//        int i = 0;
//        Parser parser("prog.txt");
//        parser.analyze();
//        cout << "----------- POLIZ -----------" << endl;
//        while (i < parser.poliz.size()) {
//            cout << i << ": " << parser.poliz.at(i);
//            i++;
//        }
    }
    catch (const char* c) { cout << c << endl; }
    catch (Lex l) { cout << "invalid lexem " << l;}
    catch (char c) { cout << "err with symbol: " << c << endl; }
    catch (...) { cout << "undefined err" << endl; }
    return 0;
}