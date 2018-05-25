#include<bits/stdc++.h>
#include<sqlite3.h>
#include"binary.hpp"
using namespace std;
using namespace sjtu;
InputOfBinary bin;
OutputOfBinary bout;
char buff[1<<20];
const string birthday="2018-04-01";
int int_len=0;
int to_int(string s){
    int x=0;
    int_len=0;
    for(unsigned i=0;i<s.length();i++){
        x=x*10+(s[i]-'0');
        int_len++;
    }
    return x;
}
double to_double(string s){
    for(int i=0;i<s.length();i++){
        if(s[i]=='.'){
            double x=to_int(s.substr(0,i));
            double y=to_int(s.substr(i+1,(int)s.length()-i-1));
            while(int_len--)y/=10;
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
        cout<<train_id<<" "<<name<<" "<<catalog<<" "<<num_station<<" "<<num_price;
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

    table="create table if not exists route( \
            id integer primary key autoincrement, \
            loc1 text, \
            loc2 text, \
            catalog text, \
            trainid text);";

    run(table);
    run("create index if not exists rindex on route(loc1,loc2,catalog);");

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
        string  start="2018-06-"+string(1,'0'+i/10)+string(1,'0'+i%10);
        sqlite3_bind_text(stmt, 5, start.c_str(), start.length(), SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    return 1;
}

void query_train(string trainid){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select info,onsale from train where trainid=(?) limit 1;";  
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
    int onsale=sqlite3_column_int(stmt,1);
    if(!onsale){
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
    static const char* sql3 = "insert into route values(NULL,?,?,?,?);";  
    
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

vector<string>vec_string;
string date_plus(string date,int x){
    "2018-06-01";
    int d=to_int(date.substr(8,2))+x;
    if(d>=31){
        date[6]='7';
        date[8]='0';
        date[9]='1';
        return date;
    }
    date[8]=d/10+'0';
    date[9]=d%10+'0';
    return date;
}
void _query_ticket(string loc1,string loc2,string date,string catalog){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select info from train where date=(?) and trainid in (select trainid from route where loc1=(?) and loc2=(?) and catalog=(?));";  
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
        int dd=0;
        for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc1)posi=i;
        for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc2)posj=i;
        for(int i=0;i<posi;i++)
            if(train.start_time[i]>train.arriv_time[i+1]){
                dd++;
            }

        ans+=loc1+" "+date_plus(date,dd)+" "+train.start_time[posi]+" ";
        if(train.start_time[posi]>train.arriv_time[posj])
            dd++;
        ans+=loc2+" "+date_plus(date,dd)+" "+train.arriv_time[posj];

        for(int i=0;i<train.num_price;i++){
            ans+=" "+train.ticket_name[i];
            double sum=0;
            int num=int(1e9);
            dd=0;
            for(int k=posi;k<posj;k++){
                sum+=train.price[k+1][i];
                num=min(num,train.ticket[k][i]);
                if(train.start_time[k]>train.arriv_time[k+1])
                    dd++;
            }
            if(dd>=2){cerr<<"trainid = "<<train.train_id<<" run more than 48h"<<endl;}
            ans+=" "+to_string(num)+" "+to_string(sum);
        }
        ans+="\n";
        vec_string.push_back(ans);
        ans="";
    }
    
}
void query_ticket(string loc1,string loc2,string date,string catalog){
	vec_string.clear();
	for(int i=0;i<catalog.length();i++)
		_query_ticket(loc1,loc2,date,catalog.substr(i,1));
    cout<<vec_string.size()<<endl;
	for(auto s:vec_string)
		cout<<s;cout<<flush;
}

int delete_train(string trainid){
	static int flag=0;
    static sqlite3_stmt *stmt,*stmt2,*stmt3;  
    static const char* sql = "select onsale from train where trainid=(?) limit 1;";  
    static const char* sql2 = "delete from train where trainid=?;";  
    static const char* sql3 = "delete from route where trainid=?;";  
    
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
    
    int onsale=sqlite3_column_int(stmt,0);
    if(onsale) 
        return 0;
    sqlite3_reset(stmt2);
    sqlite3_bind_text(stmt2, 1, trainid.c_str(),trainid.length(),SQLITE_STATIC);
    sqlite3_step(stmt2);

    sqlite3_reset(stmt3);
    sqlite3_bind_text(stmt3, 1, trainid.c_str(),trainid.length(),SQLITE_STATIC);
    sqlite3_step(stmt3);

        
    return 1;    
}
int modify_train(Train &train){
    if(!delete_train(train.train_id))
        return 0;
    if(!add_train(train))
        return 0;
    return 1;
}
bool get_train(string trainid,string date){
    static int flag=0;
    static sqlite3_stmt *stmt,*stmt2,*stmt3,*stmt4,*stmt5;  
    static const char* sql = "select info from train where date=(?) and trainid=(?);";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt,1,date.c_str(),date.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt,2,trainid.c_str(),trainid.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);

    string ans;
    if(rc!=SQLITE_ROW)
        return 0;
    train.from_blob((char*)sqlite3_column_blob(stmt,0));
    return 1;    
}
int buy_ticket(int id,int num,string trainid,string loc1,string loc2,string date,string ticket_kind){
    if(!exist_id(id))
        return 0;
    static int flag=0;
    static sqlite3_stmt *stmt,*stmt2,*stmt3,*stmt4,*stmt5;  
    static const char* sql = "select info from train where date=(?) and trainid=(?);";  
    static const char* sql2 = "update train set info=(?) where date=(?) and trainid=(?);";  
    static const char* sql3 = "select num from ticket where id=(?) and trainid=(?) and loc1=(?) and loc2=(?) and date=(?) and ticket_kind=(?);";  
    static const char* sql4 = "insert into ticket values(NULL,?,?,?,?,?,?,?,?);";  
    static const char* sql5 = "update ticket set num=num+(?) where id=(?) and trainid=(?) and loc1=(?) and loc2=(?) and date=(?) and ticket_kind=(?);"; 
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
        sqlite3_prepare_v2(db,sql2,strlen(sql2),&stmt2,0); 
        sqlite3_prepare_v2(db,sql3,strlen(sql3),&stmt3,0); 
        sqlite3_prepare_v2(db,sql4,strlen(sql4),&stmt4,0); 
        sqlite3_prepare_v2(db,sql5,strlen(sql5),&stmt5,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt,1,date.c_str(),date.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt,2,trainid.c_str(),trainid.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);

    string ans;
    if(rc!=SQLITE_ROW)
        return 0;
    train.from_blob((char*)sqlite3_column_blob(stmt,0));
    ans+=train.train_id+" ";
    int posi=-1,posj=-1;
    for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc1)posi=i;
    for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc2)posj=i;
    if(posi==-1 || posj==-1 || posi>=posj)
        return 0;
        
    double sum=0;
    int nums=int(1e9);
    int kind=-1;
    for(int i=0;i<train.num_price;i++)if(train.ticket_name[i]==ticket_kind)kind=i;
    if(kind==-1)return 0;
    for(int k=posi;k<posj;k++){
        sum+=train.price[k][kind];
        nums=min(nums,train.ticket[k][kind]);
    }
    
    if(num>=0 && nums<num)
        return 0;
    
    for(int k=posi;k<posj;k++)
        train.ticket[k][kind]-=num;
    
    sqlite3_reset(stmt3);
    sqlite3_bind_int(stmt3,1,id);
    sqlite3_bind_text(stmt3,2,trainid.c_str(),trainid.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt3,3,loc1.c_str(),loc1.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt3,4,loc2.c_str(),loc2.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt3,5,date.c_str(),date.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt3,6,ticket_kind.c_str(),ticket_kind.length(),SQLITE_STATIC);
    rc=sqlite3_step(stmt3);
    int onum=0;
    if(rc==SQLITE_ROW){
        onum=sqlite3_column_int(stmt3,0);
    }
    if(num < 0 && onum<-num)
        return 0;

    char *buff=train.to_blob();
    sqlite3_reset(stmt2);
    sqlite3_bind_blob(stmt2,1,buff,bout.fout.cur-buff,0);
    sqlite3_bind_text(stmt2,2,date.c_str(),date.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt2,3,trainid.c_str(),trainid.length(),SQLITE_STATIC);
    sqlite3_step(stmt2);


    if(rc!=SQLITE_ROW){
        sqlite3_reset(stmt4);
        sqlite3_bind_int(stmt4,1,id);
        sqlite3_bind_int(stmt4,2,num);
        sqlite3_bind_text(stmt4,3,trainid.c_str(),trainid.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt4,4,loc1.c_str(),loc1.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt4,5,loc2.c_str(),loc2.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt4,6,date.c_str(),date.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt4,7,train.catalog.c_str(),train.catalog.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt4,8,ticket_kind.c_str(),ticket_kind.length(),SQLITE_STATIC);
        sqlite3_step(stmt4);
    }else{ 
        sqlite3_reset(stmt5);
        sqlite3_bind_int(stmt5,1,num);
        sqlite3_bind_int(stmt5,2,id);
        sqlite3_bind_text(stmt5,3,trainid.c_str(),trainid.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt5,4,loc1.c_str(),loc1.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt5,5,loc2.c_str(),loc2.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt5,6,date.c_str(),date.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt5,7,ticket_kind.c_str(),ticket_kind.length(),SQLITE_STATIC);
        sqlite3_step(stmt5);
    }

    return 1;

}

void _query_order(int id,string date,string catalog){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select * from ticket where date=(?) and catalog=(?);";  
    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
    }
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, date.c_str(),date.length(),SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, catalog.c_str(),catalog.length(),SQLITE_STATIC);
    int rc=sqlite3_step(stmt);

    string ans;
    if(rc!=SQLITE_ROW){
        return ;
    }
    int cnt=0;
    string dt;
    while(rc==SQLITE_ROW){
        cnt++;
        int num=sqlite3_column_int(stmt,2);
        string trainid=(const char*)sqlite3_column_text(stmt,3);
        string loc1=(const char*)sqlite3_column_text(stmt,4);
        string loc2=(const char*)sqlite3_column_text(stmt,5);
        string date=(const char*)sqlite3_column_text(stmt,6);
        string catalog=(const char*)sqlite3_column_text(stmt,7);
        string ticket_kind=(const char*)sqlite3_column_text(stmt,8);


        if(get_train(trainid,date)){
            ans+=train.train_id+" ";
            int posi=-1,posj=-1;
            for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc1)posi=i;
            for(int i=0;i<train.num_station;i++)if(train.station_name[i]==loc2)posj=i;
            ans+=loc1+" "+date+" "+train.start_time[posi]+" ";
            ans+=loc2+" "+date+" "+train.arriv_time[posj];
            for(int i=0;i<train.num_price;i++)if(ticket_kind==train.ticket_name[i]){
                ans+=" "+train.ticket_name[i];
                double sum=0;
                for(int k=posi;k<posj;k++){
                    sum+=train.price[k+1][i];
                }
                ans+=" "+to_string(num)+" "+to_string(sum);
            }
            ans+="\n";
            vec_string.push_back(ans);
            ans="";
        }else {
            return ;
        }


        rc=sqlite3_step(stmt);
    }
}

void query_order(int id,string date,string catalog){
	vec_string.clear();
	for(int i=0;i<catalog.length();i++)
		_query_order(id,date,catalog.substr(i,1));
    cout<<vec_string.size()<<endl;
	for(auto s:vec_string)
		cout<<s;cout<<flush;
}
void query_transfer(string loc1,string loc2,string date,string catalog){
    static int flag=0;
    static sqlite3_stmt *stmt;  
    static const char* sql = "select distinct trainid from route where loc1=(?) and catalog=(?)  union select distinct trainid from route where loc2=(?) and catalog=(?)";  
    static sqlite3_stmt *stmt2;  
    static const char* sql2 = "select info from train where date=(?) and trainid=(?);";  

    if(!flag){
        flag=1;
        sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0); 
        sqlite3_prepare_v2(db,sql2,strlen(sql2),&stmt2,0); 
    }
    
    string ans;

	vector<string>tids;
	vector<Train>trains;    
    int rc;
    for(int ix=0;ix<catalog.length();ix++){
		string cata=catalog.substr(ix,1);		
		sqlite3_reset(stmt);
		sqlite3_bind_text(stmt, 1, loc1.c_str(),loc1.length(),SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, cata.c_str(),cata.length(),SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, loc2.c_str(),loc2.length(),SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, cata.c_str(),cata.length(),SQLITE_STATIC);
		rc=sqlite3_step(stmt);


		while(rc==SQLITE_ROW){
		    string tmp=(const char *)sqlite3_column_text(stmt,0);
		    tids.push_back(tmp);
		    rc=sqlite3_step(stmt);
		}
		
	}

    Train t; 
    for(auto trainid : tids){  
        sqlite3_reset(stmt2);
        sqlite3_bind_text(stmt2, 1, date.c_str(),date.length(),SQLITE_STATIC);
        sqlite3_bind_text(stmt2, 2, trainid.c_str(),trainid.length(),SQLITE_STATIC);
        rc=sqlite3_step(stmt2);
        if(rc==SQLITE_ROW){
            t.from_blob((char*)sqlite3_column_blob(stmt2,0));
            trains.push_back(t);
        }
    }
    map<string,pair<string,string> >min_arriv;
    map<string,pair<string,string> >min_dest;
    for(auto &train : trains){
        int pos=-1;
        for(int i=0;i<train.num_station;i++){
            if(train.station_name[i]==loc1)
                pos=i;
        }
        if(pos==-1)continue;
        if(min_arriv.count(loc1)){
            min_arriv[loc1]=min(min_arriv[loc1],make_pair(train.start_time[pos],train.train_id));
        }else min_arriv[loc1]=make_pair(train.start_time[pos],train.train_id);
        for(int i=pos+1;i<train.num_station;i++){
            auto &p=min_arriv[train.station_name[i]];
            if(p.first!=""){
                p=min(p,make_pair(train.arriv_time[i],train.train_id));
            }else p=make_pair(train.arriv_time[i],train.train_id);
        }
    }


    for(auto &train : trains){
        int pos=-1;
        for(int i=0;i<train.num_station;i++){
            if(train.station_name[i]==loc2)
                pos=i;
        }
        if(pos==-1)continue;
        
        for(int i=0;i<pos;i++){
            if(!min_arriv.count(train.station_name[i]))continue;
            auto &p=min_arriv[train.station_name[i]];
            if(train.start_time[i]>p.first){
                auto &d=min_dest[train.station_name[i]];
                if(d.first=="")d=make_pair(train.arriv_time[pos],train.train_id);
                else d=min(d,make_pair(train.arriv_time[pos],train.train_id));
            }
        }
    }


    if(min_dest.empty()){
        cout<<-1<<endl;
        return ;
    }
    string mn=min_dest.begin()->first;
    string mid;
    for(auto &pr : min_dest){
        if(pr.second.first < mn){
            mn=pr.second.first;
            mid=pr.first;
        }
    }

    static auto find_way=[&](Train &train,string loc1,string loc2) -> string{
        int posi=-1,posj=-1;
        for(int i=0;i<train.num_station;i++){
            if(train.station_name[i]==loc1)posi=i;
            if(train.station_name[i]==loc2)posj=i;
        }
        string ans;
        ans+=train.train_id+" ";
        ans+=loc1+" "+date+" "+train.start_time[posi]+" ";
        ans+=loc2+" "+date+" "+train.arriv_time[posj];
        for(int i=0;i<train.num_price;i++){
            ans+=" "+train.ticket_name[i];
            double sum=0;
            int num=int(1e9);
            for(int k=posi;k<posj;k++){
                sum+=train.price[k+1][i];
                num=min(num,train.ticket[k][i]);
            }
            ans+=" "+to_string(num)+" "+to_string(sum);
        }
        return ans;
    };
    string ans1,ans2;
    for(auto &train : trains){
        if(train.train_id==min_arriv[mid].second){
            ans1=find_way(train,loc1,mid);
        }
        if(train.train_id==min_dest[mid].second){
            ans2=find_way(train,mid,loc2);
        }
    }
    cout<<ans1<<endl;
    cout<<ans2<<endl;
}
int main(){
    init_database();
    string cmd;
    try_start();
    while(cin>>cmd){
        
        //cerr<<cmd<<endl;
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
            string loc1,loc2,date,catalog;
            cin>>loc1>>loc2>>date>>catalog;
            query_transfer(loc1,loc2,date,catalog);
        }else
        if(cmd=="buy_ticket"){
            string train_id,loc1,loc2,date,ticket_kind;
            int id,num;
            cin>>id>>num>>train_id>>loc1>>loc2>>date>>ticket_kind;
            cout<<buy_ticket(id,num,train_id,loc1,loc2,date,ticket_kind)<<endl;
        }else
        if(cmd=="query_order"){
            int id;
            string date,catalog;
            cin>>id>>date>>catalog;
            query_order(id,date,catalog);
        }else
        if(cmd=="refund_ticket"){
            string train_id,loc1,loc2,date,ticket_kind;
            int id,num;
            cin>>id>>num>>train_id>>loc1>>loc2>>date>>ticket_kind;
            cout<<buy_ticket(id,-num,train_id,loc1,loc2,date,ticket_kind)<<endl;
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
            string trainid;
            cin>>trainid;
            cout<<delete_train(trainid)<<endl;
        }else
        if(cmd=="modify_train"){
            train.read();
            cout<<modify_train(train)<<endl;

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
