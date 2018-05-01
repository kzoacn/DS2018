#include <iostream>  
#include <string>  
#include <sstream>  
#include <time.h>  
#include <cstring>
#include "sqlite3.h"  
  
const int nCount = 10000;  
      
int main (int argc,char** argv)  
{  
    sqlite3* db;  
    sqlite3_open("testdb.db" ,&db);  
    sqlite3_exec(db,"PRAGMA synchronous = OFF; ",0,0,0);  
    sqlite3_exec(db,"drop table if exists t1",0,0,0);  
    sqlite3_exec(db,"create table t1(id integer,x integer,y integer ,weight real)",0,0,0);  
    clock_t t1 = clock();  
      
    //sqlite3_exec(db,"begin;",0,0,0);  
    sqlite3_stmt *stmt;  
    const char* sql = "insert into t1 values(?,?,?,?)";  
    sqlite3_prepare_v2(db,sql,strlen(sql),&stmt,0);  
      
    for(int i=0;i<nCount;++i)  
    {  
        // std::stringstream ssm;  
        // ssm<<"insert into t1 values("<<i<<","<<i*2<<","<<i/2<<","<<i*i<<")";  
        // sqlite3_exec(db,ssm.str().c_str(),0,0,0);  
        sqlite3_reset(stmt);  
        sqlite3_bind_int(stmt,1,i);  
        sqlite3_bind_int(stmt,2,i*2);  
        sqlite3_bind_int(stmt,3,i/2);  
        sqlite3_bind_double(stmt,4,i*i);  
        sqlite3_step(stmt);  
    }  
    sqlite3_finalize(stmt);  
    //sqlite3_exec(db,"commit;",0,0,0);  
    clock_t t2 = clock();  
      
    sqlite3_close(db);  
      
    std::cout<<"cost tima: "<<1.0*(t2-t1)/CLOCKS_PER_SEC<<"s"<<std::endl;  
      
    return 0;  
} 
