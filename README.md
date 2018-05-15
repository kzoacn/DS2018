### 环境
	sudo apt install libsqlite3*
	
### 编译
    g++ train.cpp -o train -std=c++11 -l sqlite3
    g++ special_judge.cpp -o spj -std=c++11
    
### 运行

#### train
    ./train <input >output

#### special_judge
    ./spj input output answer


