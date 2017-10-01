#include<iostream>
#include<stdio.h>
#include<map>
#include<set>
#include<string>
#include<pthread.h>
#define NUM_THREAD 36
using namespace std;

class MyKey {
private:
	int startNum;
	int endNum;

public:
	MyKey() {
		startNum = 0;
		endNum = 0;
	}
	MyKey(int startNum, int endNum) {
		this->startNum = startNum;
		this->endNum = endNum;
	}

	int getStartNum() const {
		return this->startNum;
	}

	int getEndNum() const {
		return this->endNum;
	}
};

struct MyKeyCompare {
	bool operator() (const MyKey& lhs, const MyKey& rhs) const {
		if(lhs.getStartNum() == rhs.getStartNum()) {
			return lhs.getEndNum() < rhs.getEndNum();
		} else {
			return lhs.getStartNum() < rhs.getStartNum();
		}
	}
};

multimap<MyKey, string, MyKeyCompare> result;
set<string> word_list;
string buf;
int word_split_num;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool thread_done[NUM_THREAD];

void* ThreadFunc(void* arg) {
	long tid = (long)arg;

    pthread_mutex_lock(&my_mutex);
    thread_done[tid] = false;
    pthread_cond_wait(&cond, &my_mutex);
    pthread_mutex_unlock(&my_mutex);

    while(1) {
        if((size_t)tid >= word_list.size() ||
        (size_t)tid * word_split_num >= word_list.size()) {
            pthread_mutex_lock(&my_mutex);
            thread_done[tid] = true;
            pthread_cond_wait(&cond, &my_mutex);
            pthread_mutex_unlock(&my_mutex);
        } else {
        	set<string>::iterator start_iter = word_list.begin();
        	set<string>::iterator end_iter;
        	set<string>::iterator it;
            multimap<MyKey, string, MyKeyCompare> thread_result;

        	advance(start_iter, tid * word_split_num);
        	end_iter = start_iter;
        	advance(end_iter, word_split_num);

        	//cout << "this is thread : " << tid << endl;

        	if(tid == NUM_THREAD -1 || (word_list.size()) < (((size_t)tid+1) * word_split_num)) {
        		end_iter = word_list.end();
        	}

        	for(it = start_iter; it != end_iter; it++) {
        		size_t startNum = buf.find(*it);

                	if (startNum != string::npos){
                		size_t endNum = startNum + (*it).length();        	   
        	            thread_result.insert(make_pair(MyKey((int)startNum, (int)endNum), *it));
                        /*pthread_mutex_lock(&my_mutex);
                        result.insert(make_pair(MyKey((int)startNum, (int)endNum), *it));
                        pthread_mutex_unlock(&my_mutex);*/
        	        }
        	}

            pthread_mutex_lock(&my_mutex);
            for(multimap<MyKey, string, MyKeyCompare>::iterator mit = thread_result.begin(); mit != thread_result.end(); mit++) {
                result.insert(make_pair(MyKey((mit->first).getStartNum(), (mit->first).getEndNum()), mit->second));
            }
            thread_done[tid] = true;
            pthread_cond_wait(&cond, &my_mutex);
            pthread_mutex_unlock(&my_mutex);
        }
    }

	return NULL;
}

int main(void) {
    int N;
    char cmd;
    pthread_t threads[NUM_THREAD];
    std::ios::sync_with_stdio(false);

    cin >> N;
    for (int i = 0; i < N; i++){
        cin >> buf;
        word_list.insert(buf);
    }
    cout << "R" << std::endl;

    for(long i = 0; i < NUM_THREAD; i++) {
        if(pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
            printf("pthread_create error!\n");
            return 0;
        }

        while (thread_done[i] != false) {
            pthread_yield();
        }
    }

    while(cin >> cmd){
        cin.get();
        getline(cin, buf);
        switch(cmd){
            case 'Q': 
                {
                    result.clear();
                    word_split_num = (word_list.size() / NUM_THREAD) + 1;

                    for(int i = 0;i<NUM_THREAD;i++) {
                        thread_done[i] = false;
                    }
                    
                    pthread_mutex_lock(&my_mutex);
                    pthread_cond_broadcast(&cond);
                    pthread_mutex_unlock(&my_mutex);
                    
                    while (1) {
                        bool all_thread_done = true;
                        for (int i = 0; i < NUM_THREAD; i++) {
                            if (!thread_done[i]) {
                                all_thread_done = false;
                                break;
                            }
                        }
                        if (all_thread_done) {
                            break;
                        }
                        pthread_yield();
                    }
        
                    multimap<MyKey, string, MyKeyCompare>::iterator it = result.begin();
                    for (int cnt = result.size(); cnt != 0; cnt--, it++){
                        cout << it->second;
                        if (cnt != 1){
                            cout << "|";
                        }
                    }

                    if(result.size() == 0) {
                        cout << "-1";
                    }
                    cout << std::endl;
                }
                break;
            case 'A':
                word_list.insert(buf);
                break;
            case 'D':
                word_list.erase(buf);
                break;
        }
        
    }

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

