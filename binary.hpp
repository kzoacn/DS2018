
#ifndef SJTU_FILEMANAGER_HPP
#define SJTU_FILEMANAGER_HPP


#include <fstream>
#include <cstring>
#include <iostream>
#include<vector>
#include<algorithm>


namespace sjtu {


    using std::vector;
    using std::pair;
    using std::string;
    using std::ifstream;
    using std::ofstream;
    class mystream{
    private:
        char* cur;
    public:
        void open(char *buff){
            cur=buff;
        }
        void read(char* a,long len){
            for(int i=0;i<len;i++){
                *(a+i)=*cur;
                cur++;
            }
        }
        void write(char *a,long len){
            for(int i=0;i<len;i++){
                *cur=*(a+i);
                cur++;
            }
        }

    };

	class InputOfBinary {

		private:
			mystream fin;
		
		public:


        void open(char *s){fin.open(s);}
		friend InputOfBinary & operator >> (InputOfBinary & cin, int & A) {
			cin.fin.read((char *) (& A), sizeof A);
			return cin;
		}
	
		friend InputOfBinary & operator >> (InputOfBinary & cin, double & A) {
			cin.fin.read((char *) (& A), sizeof A);
			return cin;
		}

        friend InputOfBinary & operator >> (InputOfBinary & cin, char & A) {
            cin.fin.read((char *) (& A), sizeof A);
            return cin;
        }
        friend InputOfBinary & operator >> (InputOfBinary & cin, bool & A) {
            cin.fin.read((char *) (& A), sizeof A);
            return cin;
        }
	
        friend InputOfBinary & operator >> (InputOfBinary & cin, string & A) {
            int len;
            cin>>len;
            A="";
            for(int i=0;i<len;i++){
                char c;cin>>c;
                A+=c;
            }
            return cin;
        }

        template<class T1,class T2>
        friend InputOfBinary & operator >> (InputOfBinary & cin, pair<T1,T2> & A) {
            cin>>A.first>>A.second;
            return cin;
        }

        template<class T>
        friend InputOfBinary & operator >> (InputOfBinary & cin, vector<T> & A) {
            int len;
            cin>>len;
            A.clear();
            for(int i=0;i<len;i++){
                T c;cin>>c;
                A.push_back(c);
            }
            return cin;
        }

		~InputOfBinary() {
        }
	
    };

	class OutputOfBinary {

		private:
			mystream fout;
		
		public:
		
        void open(char *s){fout.open(s);}
	
		friend OutputOfBinary & operator << (OutputOfBinary & cout, const int & A) {
			cout.fout.write((char *) (& A), sizeof A);
			return cout;
		}
	
		friend OutputOfBinary & operator << (OutputOfBinary & cout, const double & A) {
			cout.fout.write((char *) (& A), sizeof A);
			return cout;
		}

        friend OutputOfBinary & operator << (OutputOfBinary & cout, const char & A) {
            cout.fout.write((char *) (& A), sizeof A);
            return cout;
        }
        friend OutputOfBinary & operator << (OutputOfBinary & cout, const bool & A) {
            cout.fout.write((char *) (& A), sizeof A);
            return cout;
        }
	
        friend OutputOfBinary & operator << (OutputOfBinary & cout, const string & A) {
            cout<<(int)A.length();
            for(int i=0;i<A.length();i++)
                cout<<A[i];
            return cout;
        }

        template<class T>
        friend OutputOfBinary & operator << (OutputOfBinary & cout, const vector<T> & A) {
            cout<<(int)A.size();
            for(int i=0;i<A.size();i++)
                cout<<(T)A.at(i);
            return cout;
        }



        template<class T1,class T2>
        friend OutputOfBinary & operator << (OutputOfBinary & cout, const pair<T1,T2> & A) {
            cout<<(T1)A.first<<(T2)A.second;
            return cout;
        }
	
		~OutputOfBinary() {
        }

	};

	
};

#endif
