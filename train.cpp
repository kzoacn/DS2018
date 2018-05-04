#include<bits/stdc++.h>
#include<sqlite3.h>
#include"binary.hpp"
using namespace std;
using namespace sjtu;
InputOfBinary bin;
OutputOfBinary bout;
char buff[1<<20];
const string birthday="2018-04-01";
int to_int(string s){
    int x=0;
    for(unsigned i=0;i<s.length();i++){
        x=x*10+(s[i]-'0');
    }
    return x;
}
double to_double(string s){
    for(int i=0;i<s.length();i++){
        if(s[i]=='.'){
            double x=to_int(s.substr(0,i));
            double y=to_int(s.substr(i+1,(int)s.length()-i-1));
            while(y>=1)y/=10;
            return x+y;
        }
    }
    return to_int(s);
}

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

int tcount=0,sum=0;
class Train{
public:
    string train_id;
    string name;
	string catalog;
    int num_station;
    int num_price;
    vector<string>ticket_name;
    vector<string>station_name;
    vector<string>arriv_time;
    vector<string>start_time;
    vector<string>stopover_time;
    vector<vector<double> >price;
    vector<vector<int> >ticket;
    char* to_blob(){
        bout.open(buff);
        bout<<train_id<<name<<catalog<<num_station<<num_price;
        bout<<ticket_name<<station_name<<arriv_time<<start_time<<stopover_time<<price<<ticket;
        return buff;
    }
    void from_blob(char* s){
        bin.open(s);
        bin>>train_id>>name>>catalog>>num_station>>num_price;
        bin>>ticket_name>>station_name>>arriv_time>>start_time>>stopover_time>>price>>ticket;
    }

	void read(){
        //if(++tcount > 5)exit(0);
        tcount++;
		cin>>train_id;
		cin>>name;
		cin>>catalog;
		cin>>num_station>>num_price;
		ticket_name.resize(num_price);
		station_name.resize(num_station);
		arriv_time.resize(num_station);
		start_time.resize(num_station);
        stopover_time.resize(num_station);
        price.resize(num_station);
        for(auto &vec : price)
            vec.resize(num_price);
        ticket.resize(num_station);
        for(auto &vec : ticket)
            vec.resize(num_price);
		for(int i=0;i<num_price;i++){
			cin>>ticket_name[i];
        }
		for(int i=0;i<num_station;i++){
            cin>>station_name[i];
            cin>>arriv_time[i]>>start_time[i]>>stopover_time[i];
            for(int j=0;j<num_price;j++){
                string pri;cin>>pri;
                price[i][j]=to_double(pri.substr(3,pri.length()-3));
                ticket[i][j]=2000;
            }
        }

	}
    void print(){
        cout<<fixed<<setprecision(2);
        cout<<train_id<<" "<<name<<" "<<num_station<<" "<<num_price;
        for(auto s:ticket_name)cout<<" "<<s;cout<<endl;
		for(int i=0;i<num_station;i++){
            cout<<station_name[i]<<" ";
            cout<<arriv_time[i]<<" "<<start_time[i]<<" "<<stopover_time[i]<<" ";
            for(int j=0;j<num_price;j++){
                cout<<" ￥"<<price[i][j];
            }
            cout<<endl;
        }        
    }
}train;




sqlite3 *db;
vector<vector<string> >select_table;
vector<string>select_col;
int inner_id;
bool Record;
bool Debug=false;
bool inserting=false;

void try_start(){
    if(!inserting){
        inserting=true;
        sqlite3_exec(db,"begin;",0,0,0);
    }
}
void try_close(){
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

void init_database(){
    int rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_exec(db,"PRAGMA synchronous = OFF; ",0,0,0);  
    //sqlite3_exec(db,"drop table if exists User",0,0,0);  
    string table;

    table="Create Table if not exists user( "
            "inner_id   int primary key   not NULL, "
            "id         int               not NULL, "
            "name       text              not NULL, "
            "password   text              not NULL, "
            "email      text              not NULL, "
            "phone      text              not NULL, "
            "privilege  int               not NULL)";


    run(table);
    run("create unique index if not exists idIndex on user(id);");

    table="create table if not exists routine( \
            id integer primary key autoincrement, \
            loc1 text, \
            loc2 text, \
            catalog text, \
            trainid text);";

    run(table);
    run("create index if not exists rindex on routine(loc1,loc2,catalog);");

    table="create table if not exists ticket( \
            buyid integer primary key autoincrement, \
            id int, \
            num int, \
            trainid text,\
            loc1 text,\
            loc2 text,\
            date text,\
            catalog text,\
            ticket_kind text);";

    run(table);
    run("create index if not exists tindex1 on ticket(id,date,catalog);");
    run("create index if not exists tindex2 on ticket(id,date,trainid,loc1,loc2,ticket_kind);");

    table="create table if not exists train( \
            id integer primary key autoincrement, \
            trainid text , \
            onsale int, \
            sold int, \
            info blob, \
            date text \
        );";

    run(table);
    run("create index if not exists trindex on train(trainid);");



    select_table.clear();
    run("select count(*) from user;",true);
    inner_id=0;
    for(int i=0;i<select_table[0][0].length();i++)
        inner_id=inner_id*10+(select_table[0][0][i]-'0');
}


int regist(string name,string password,string email,string phone){
 
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "insert into user values(?,?,?,?,?,?,?);";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
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

    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select name,email,phone,privilege from user where id=(?) limit 1;";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);
    int rc=sqlite3_step(stmt);
    
    string ans;
    if(rc!=SQLITE_ROW)
        return "0";
//inner_id,id,name,password,email,phone,privilege
//   0      1  2     3        4    5      6  
    ans+=(const char*)sqlite3_column_text(stmt,0);ans+=" ";
    ans+=(const char*)sqlite3_column_text(stmt,1);ans+=" ";
    ans+=(const char*)sqlite3_column_text(stmt,2);ans+=" ";
    ans+=to_string(sqlite3_column_int(stmt,3));
    return ans;
}

bool exist_id(int id){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select id from user where id=(?) limit 1;";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);
    int rc=sqlite3_step(stmt);
    
    string ans;
    if(rc!=SQLITE_ROW)
        return false;
    return true;
}
bool exist_trainid(string id){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select id from train where trainid=(?) limit 1;";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, id.c_str(),id.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);
    
    string ans;
    if(rc!=SQLITE_ROW)
        return false;
    return true;
}
int modify_profile(int id,string name,string password,string email,string phone){
    if(!exist_id(id))
        return 0;
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "update user set name=(?),password=(?),email=(?),phone=(?) where id=(?);";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), password.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email.c_str(), email.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, phone.c_str(), phone.length(), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, id);
    sqlite3_step(stmt);
        
    return 1;
}

int get_privilege(int id){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select privilege from user where id=(?) limit 1;";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, id);
    int rc=sqlite3_step(stmt);
    int ans=0;
    if(rc!=SQLITE_ROW)
        return 0;
    
    ans=to_int(to_string(sqlite3_column_int(stmt,0)));
    return ans;
}

int modify_privilege(int id1,int id2,int privilege){
    int pri1=(id1==2018) ? 2 :get_privilege(id1);
    if(pri1!=2)
        return 0;
    int pri2=get_privilege(id2);
    if(pri2==2 || pri2==0)
        return 0;
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "update user set privilege=(?) where id=(?);";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_int(stmt, 1, privilege);
    sqlite3_bind_int(stmt, 2, id2);
    sqlite3_step(stmt);
        
    return 1;
}

int clean(){
    try_close();
    run("drop table user");
    return 1;
}

int add_train(Train &train){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "insert into train values(NULL,?,?,?,?,?);";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    for(int i=1;i<=30;i++){
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, train.train_id.c_str(), train.train_id.length(), SQLITE_STATIC);
        sqlite3_bind_int(stmt,2,0);
        sqlite3_bind_int(stmt,3,0);
        char *buff=train.to_blob();
        sqlite3_bind_blob(stmt,4,buff,bout.fout.cur-buff,0);
        string  start="2018-04-"+string(1,'0'+i/10)+string(1,'0'+i%10);
        sqlite3_bind_text(stmt, 5, start.c_str(), start.length(), SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    return 1;
}

void query_train(string trainid){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select info from train where trainid=(?) limit 1;";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, trainid.c_str(),trainid.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);
    
    string ans;
    if(rc!=SQLITE_ROW){
        cout<<"0"<<endl;
        return ;
    }
    train.from_blob((char*)sqlite3_column_blob(stmt,0));
    train.print();
}

int sale_train(string trainid){
    static int flag=0;
    static sqlite3_stmt *stmt,*stmt2,*stmt3;  
    static const char* sql = "select info,onsale from train where trainid=(?) limit 1;";  
    static const char* sql2 = "update train set onsale=1 where trainid=(?) ;";  
    static const char* sql3 = "insert into routine values(NULL,?,?,?,?);";  
    
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
        sqlite3_prepare_v2(db,sql2,strlen(sql2),&stmt2,0); 
        sqlite3_prepare_v2(db,sql3,strlen(sql3),&stmt3,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, trainid.c_str(),trainid.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);
    
    string ans;
    if(rc!=SQLITE_ROW){
        return 0;
    }
    train.from_blob((char*)sqlite3_column_blob(stmt,0));
    int onsale=sqlite3_column_int(stmt,1);
    if(onsale) 
        return 0;
    sqlite3_reset(stmt2);
    sqlite3_bind_text(stmt2, 1, trainid.c_str(),trainid.length(),SQLITE_STATIC);
    sqlite3_step(stmt2);

    for(int i=0;i<train.num_station;i++){
        for(int j=i+1;j<train.num_station;j++){
            sqlite3_reset(stmt3);
            sqlite3_bind_text(stmt3, 1, train.station_name[i].c_str(),train.station_name[i].length(),SQLITE_STATIC);
            sqlite3_bind_text(stmt3, 2, train.station_name[j].c_str(),train.station_name[j].length(),SQLITE_STATIC);
            sqlite3_bind_text(stmt3, 3, train.catalog.c_str(),train.catalog.length(),SQLITE_STATIC);
            sqlite3_bind_text(stmt3, 4, train.train_id.c_str(),train.train_id.length(),SQLITE_STATIC);
            sqlite3_step(stmt3);
        }
    }
        
    return 1;
}
void query_ticket(string loc1,string loc2,string date,string catalog){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select info from train where date=(?) and trainid in (select trainid from routine where loc1=(?) and loc2=(?) and catalog=(?));";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, date.c_str(),date.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, loc1.c_str(),loc1.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, loc2.c_str(),loc2.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, catalog.c_str(),catalog.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);

    string ans;
    if(rc!=SQLITE_ROW){
        cout<<"0"<<endl;
        return ;
    }
    int cnt=0;
    string dt;
    while(rc==SQLITE_ROW){
        cnt++;
        train.from_blob((char*)sqlite3_column_blob(stmt,0));
        rc=sqlite3_step(stmt);
        ans+=train.train_id+" ";
        int posi=-1,posj=-1;
        for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc1)posi=i;
        for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc2)posj=i;
        ans+=loc1+" "+date+" "+train.start_time[posi]+" ";
        ans+=loc2+" "+date+" "+train.arriv_time[posj];
        for(int i=0;i<train.num_price;i++){
            ans+=" "+train.ticket_name[i];
            double sum=0;
            int num=int(1e9);
            for(int k=posi;k<posj;k++){
                sum+=train.price[k][i];
                num=min(num,train.ticket[k][i]);
            }
            ans+=" "+to_string(num)+" "+to_string(sum);
        }
        ans+="\n";
    }
    cout<<cnt<<endl;
    cout<<ans;
}
int main(){

    init_database();
    string cmd;
    try_start();
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
            string loc1,loc2,date,catalog;
            cin>>loc1>>loc2>>date>>catalog;
            query_ticket(loc1,loc2,date,catalog);
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
            train.read();
            cout<<add_train(train)<<endl;
        }else
        if(cmd=="sale_train"){
            string trainid;
            cin>>trainid;
            cout<<sale_train(trainid)<<endl;
        }else
        if(cmd=="query_train"){
            string trainid;
            cin>>trainid;
            query_train(trainid);
        }else
        if(cmd=="delete_train"){

        }else
        if(cmd=="modify_train"){

        }else
        if(cmd=="clean"){
            
            cout<<clean()<<endl;
        }else
        if(cmd=="exit"){
            try_close();
            cout<<"BYE"<<endl;
            break;
        }else{
            puts("Wrong Command");
        }
    }
    sqlite3_close(db);
    //for(auto s:K)cout<<s<<endl;
    return 0;
}
