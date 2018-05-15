#include<bits/stdc++.h>
#include"binary.hpp"
using namespace std;
using namespace sjtu;

ifstream fin,fans,fout;



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

const double eps=1e-6;

bool equal(double x,double y){
    if(fabs(x-y)<eps)return 1;
    if(fabs(x-y)/max(fabs(x),fabs(y))<eps)return 1;
    return 0;
}
void myassert(bool x){
    if(!x){
        cerr<<"difference at output : " << fout.tellg() << " answer : " << fans.tellg() <<" bytes "<<endl;
        exit(-1);
    }
}
void assert_line(){
    string s1,s2;
    getline(fout,s1);
    getline(fans,s2);
    myassert(s1==s2);
}
vector<string>split(string s){
    vector<string>vec;
    string tmp;
    for(int i=0;i<(int)s.length();i++){
        if(s[i]==' '){
            if(tmp!="")
                vec.push_back(tmp);
            tmp="";
        }else tmp+=s[i];
    }
    if(tmp!="")
        vec.push_back(tmp);
    return vec;
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

	void read(ifstream &cin){
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
    #define cin fin
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
    bool equal(Train &oth){
        myassert(train_id==oth.train_id);
        myassert(name==oth.name);
        myassert(num_station==oth.num_station);
        myassert(num_price==oth.num_price);
        myassert(ticket_name==oth.ticket_name);
        for(int i=0;i<num_station;i++)
        for(int j=0;j<num_price;j++)
            myassert(::equal(price[i][j],oth.price[i][j]));
        return 1;
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



vector<vector<string> >select_table;
vector<string>select_col;
int inner_id;
bool Record;
bool Debug=false;
bool inserting=false;


void query_train(string trainid){
    static Train t1,t2;
    t1.read(fout);
    t2.read(fans);
    myassert(t1.equal(t2));
}

map<pair<string,string>,int>get_order(ifstream &fin){
    map<pair<string,string>,int>M;

    int n;
    string s;getline(fin,s);
    n=to_int(s);
    if(n==-1)n=0;
    while(n--){
        string s;getline(fin,s);
        auto vec = split(s);
        myassert(vec.size()>=10);
        string head=vec[0]+vec[1]+vec[2]+vec[3]+vec[4]+vec[5]+vec[6];
        for(int i=7;i+2<vec.size();i+=3){
            M[{head,vec[i]}]+=to_int(vec[i+1]);
        }
    }

    vector<pair<pair<string,string>,int> >del;
    for(auto pr:M){
        if(pr.second==0)del.push_back(pr);
    }
    for(auto pr:del)
        M.erase(pr.first);
    return M;
}

void query_ticket(string loc1,string loc2,string date,string catalog){
    auto m1=get_order(fout);
    auto m2=get_order(fans);
    myassert(m1==m2);
}

void query_order(int id,string date,string catalog){
    auto m1=get_order(fout);
    auto m2=get_order(fans);
    myassert(m1==m2);
}

string get_time(ifstream &fin){
    map<pair<string,string>,int>M;
    int n=2;
    for(int i=0;i<2;i++){
        string s;getline(fin,s);
        if(s=="-1")return "-1";
        auto vec = split(s);
        if(i==1)
            return vec[6];
    }
    return "xx:xx";
}
void query_transfer(string loc1,string loc2,string date,string catalog){
   //I am too lazy to fix it.
    auto t1=get_time(fout);
    auto t2=get_time(fans);
    myassert(t1==t2);
}
#define cin fin
int main(int argc,char **argv){
    if(argc!=4){
        cerr<<"./special_judge input output answer"<<endl;
        exit(0);
    }
    cerr<<"get "<<argv[1]<<" "<<argv[2]<<" "<<argv[3]<<endl;
    fin.open(argv[1]);
    fout.open(argv[2]);
    fans.open(argv[3]);

    string cmd;
    while(cin>>cmd){
        if(cmd=="register"){
            string name,password,email,phone;
            cin>>name>>password>>email>>phone;
            assert_line();
        }else
        if(cmd=="login"){
            int id;
            string password;
            cin>>id>>password;
            assert_line();
        }else
        if(cmd=="query_profile"){
            int id;
            cin>>id;
            assert_line();
        }else
        if(cmd=="modify_profile"){
            int id;
            string name,password,email,phone;
            cin>>id>>name>>password>>email>>phone;
            assert_line();
        }else
        if(cmd=="modify_privilege"){
            int id1,id2,privilege;
            cin>>id1>>id2>>privilege;
            assert_line();
        }else
        if(cmd=="query_ticket"){
            string loc1,loc2,date,catalog;
            cin>>loc1>>loc2>>date>>catalog;
            query_ticket(loc1,loc2,date,catalog);//TODO 1
        }else
        if(cmd=="query_transfer"){
            string loc1,loc2,date,catalog;
            cin>>loc1>>loc2>>date>>catalog;
            query_transfer(loc1,loc2,date,catalog);//TODO 2
        }else
        if(cmd=="buy_ticket"){
            string train_id,loc1,loc2,date,ticket_kind;
            int id,num;
            cin>>id>>num>>train_id>>loc1>>loc2>>date>>ticket_kind;
            assert_line();
        }else
        if(cmd=="query_order"){
            int id;
            string date,catalog;
            cin>>id>>date>>catalog;
            query_order(id,date,catalog);//TODO 3
        }else
        if(cmd=="refund_ticket"){
            string train_id,loc1,loc2,date,ticket_kind;
            int id,num;
            cin>>id>>num>>train_id>>loc1>>loc2>>date>>ticket_kind;
            assert_line();
        }else
        if(cmd=="add_train"){
            train.read();
            assert_line();
        }else
        if(cmd=="sale_train"){
            string trainid;
            cin>>trainid;
            assert_line();
        }else
        if(cmd=="query_train"){
            string trainid;
            cin>>trainid;
            query_train(trainid);//TODO 4
        }else
        if(cmd=="delete_train"){
            string trainid;
            cin>>trainid;
            assert_line();
        }else
        if(cmd=="modify_train"){
            train.read();
            assert_line();
        }else
        if(cmd=="clean"){
            assert_line();
        }else
        if(cmd=="exit"){
            //cout<<"BYE"<<endl;
            assert_line();
        }else{
            puts("Wrong Command");
        }
    }
    fin.close();
    fout.close();
    fans.close();
    return 0;
}
