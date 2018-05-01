#include<bits/stdc++.h>
#include<sqlite3.h>
using namespace std;


class User{
public:
    int id;
    string name,password,email,phone;
    int privilege;
};


class Date{
    int year,month,day,hour,minute;
    string to_string(){}
};

class Train{
    string train_id;
    string name;
    int num_station;
    int num_price;
    vector<string>ticket_name;
    vector<string>station_name;
    vector<Date>arriv_time;
    vector<Date>start_time;
    vector<Date>stopver_time;
    vector<vector<double> >price;
    int on_sale;
};


sqlite3 *db;
vector<vector<string> >select_table;
vector<string>select_col;
int inner_id;
bool Record;
bool Debug=false;
bool inserting=false;

void try_insert(){
    if(!inserting){
        inserting=true;
        sqlite3_exec(db,"begin;",0,0,0);
    }
}
void try_close_insert(){
    if(inserting){
        inserting=false;
        sqlite3_exec(db,"commit;",0,0,0);
    }
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    if(Record){
        select_col.clear();
        vector<string>select;
        for(int i=0; i<argc; i++){
            select.push_back(argv[i] ? argv[i] : "NULL");
            select_col.push_back(azColName[i]);
        }

        select_table.push_back(select);
    }
    if(Debug){
        for(int i=0; i<argc; i++){
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
    }
    return 0;
}

void run(string cmd,bool record=false){
    /*if(cmd.length()>7 && cmd.substr(0,6)!="insert"){
        try_close_insert();
    }
    if(cmd.length()>7 && cmd.substr(0,6)=="insert"){
        try_insert();
    }*/
    Record=record;
    char *zErrMsg = 0;

    int rc = sqlite3_exec(db, cmd.c_str(), callback, 0, &zErrMsg);
    
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        //fprintf(stderr, "Records created successfully\n");
    }    
}

void init_user(){

    //sqlite3_exec(db,"drop table if exists User",0,0,0);  
    string table;

    table="Create Table User( "
            "inner_id   int primary key   not NULL, "
            "id         int               not NULL, "
            "name       text              not NULL, "
            "password   text              not NULL, "
            "email      text              not NULL, "
            "phone      text              not NULL, "
            "privilege  int               not NULL);";


    run(table);

    //string admin;
    //admin="insert into user values(1,2018,'admin','admin','admin@t.cn','12306',2)";
    //run(admin);

    select_table.clear();
    run("select count(*) from user;",true);
    inner_id=0;
    for(int i=0;i<select_table[0][0].length();i++)
        inner_id=inner_id*10+(select_table[0][0][i]-'0');
}
void show_user(){
    string cmd;
    cmd="select * from user;";
    //cmd="select count(*) from user;";
    run(cmd);
}

    sqlite3_stmt *stmt;  
    const char* sql = "insert into user values(?,?,?,?,?,?,?);";  

int regist(string name,string password,string email,string phone){
//sqlite3_exec(db,"begin;",0,0,0);  
    inner_id++;
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, inner_id);
    sqlite3_bind_int(stmt, 2, inner_id + 2017);
    sqlite3_bind_text(stmt, 3, name.c_str(), name.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, password.c_str(), password.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, email.c_str(), email.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, phone.c_str(), phone.length(), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, (inner_id == 1 ? 2 : 1));
    sqlite3_step(stmt);
//sqlite3_exec(db,"commit;",0,0,0); 
    //sqlite3_finalize(stmt);
    return inner_id+2017;
}

int login(int id,string password){
    select_table.clear();
    select_col.clear();
    run("select * from user where id="+to_string(id),true);
    if(select_table.empty())
        return 0;
    for(int i=0;i<select_table[0].size();i++){
        if(select_col[i]=="password"){
            if(select_table[0][i]!=password){
                return 0;
            }
        }
    }
    return 1;
}

string query_profile(int id){
    select_col.clear();
    select_table.clear();
    run("select * from user where id="+to_string(id)+" limit 1",true);
    if(select_table.empty())
        return "0";
    string ans;
    for(int i=0;i<select_col.size();i++){
        if(select_col[i]=="name")
            ans+=select_table[0][i]+" ";
        if(select_col[i]=="email")
            ans+=select_table[0][i]+" ";
        if(select_col[i]=="phone")
            ans+=select_table[0][i];
    }
    return ans;
}

int modify_profile(int id,string name,string password,string email,string phone){
    select_col.clear();
    select_table.clear();
    run("select * from user where id="+to_string(id),true);
    if(select_table.empty())
        return 0;
    run("delete from user where id="+to_string(id));

    string cmd;

    cmd="insert into user values("
            +to_string(id-2017)+", "
            +to_string(id)+", "
            +"'"+name+"', "
            +"'"+password+"', "
            +"'"+email+"', "
            +"'"+phone+"', "
            +(inner_id==1?"2":"1")
            +");";
    run(cmd);
    return 1;
}
bool exist_id(int id){
    select_col.clear();
    select_table.clear();
    run("select * from user where id="+to_string(id),true);
    if(select_table.empty())
        return false;
    return true;
}
int to_int(string s){
    int x=0;
    for(unsigned i=0;i<s.length();i++){
        x=x*10+(s[i]-'0');
    }
    return x;
}
int get_privilege(int id){
    select_col.clear();
    select_table.clear();
    run("select * from user where id="+to_string(id),true);
    if(select_table.empty())
        return 0;
    int pri=0;
    for(int i=0;i<select_col.size();i++){
        if(select_col[i]=="privilege"){
            pri=to_int(select_table[0][i]);    
        }
    }
    return pri;
}

int modify_privilege(int id1,int id2,int privilege){
    if(!exist_id(id2))
        return 0;
    int pri1=get_privilege(id1);
    int pri2=get_privilege(id2);
    if(pri1!=2 || pri2==2)
        return 0;
    string cmd;
    cmd="update user set privilege="+to_string(privilege)+" where id="+to_string(id2)+";";
    run(cmd);
    return 1;
}

int clean(){
    run("drop table user");
    return 1;
}

int main(){
    int rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
sqlite3_exec(db,"PRAGMA synchronous = OFF; ",0,0,0);  
    init_user();
    show_user();
    //freopen("small.in","r",stdin);

    //ios::sync_with_stdio(false);
    //cin.tie(0);
sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0);  
    string cmd;
    while(cin>>cmd){
        
        if(cmd=="register"){
            string name,password,email,phone;
            cin>>name>>password>>email>>phone;
            cout<<regist(name,password,email,phone)<<endl;
        }else
        if(cmd=="login"){
            int id;
            string password;
            cin>>id>>password;
            cout<<login(id,password)<<endl;
        }else
        if(cmd=="query_profile"){
            int id;
            cin>>id;
            cout<<query_profile(id)<<endl;
        }else
        if(cmd=="modify_profile"){
            int id;
            string name,password,email,phone;
            cin>>id>>name>>password>>email>>phone;
            cout<<modify_profile(id,name,password,email,phone)<<endl;
        }else
        if(cmd=="modify_privilege"){
            int id1,id2,privilege;
            cin>>id1>>id2>>privilege;
            cout<<modify_privilege(id1,id2,privilege)<<endl;
        }else
        if(cmd=="query_ticket"){

        }else
        if(cmd=="query_transfer"){

        }else
        if(cmd=="buy_ticket"){

        }else
        if(cmd=="query_order"){

        }else
        if(cmd=="refund_ticket"){

        }else
        if(cmd=="add_train"){

        }else
        if(cmd=="sale_train"){

        }else
        if(cmd=="query_train"){
            
        }else
        if(cmd=="delete_train"){

        }else
        if(cmd=="modify_train"){

        }else
        if(cmd=="clean"){
            cout<<clean()<<endl;
        }else
        if(cmd=="exit"){
            try_close_insert();
            cout<<"BYE"<<endl;
            break;
        }else{
            puts("Wrong Command");
        }
    }
    sqlite3_close(db);
    return 0;
}
