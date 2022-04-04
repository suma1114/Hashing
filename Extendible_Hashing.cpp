#include <bits/stdc++.h>
using namespace std;


class Bucket {
int depth,size;
std::map<int, string> values;
public:
Bucket(int depth, int size);
int insert(int key,string value);
int remove(int key);
int update(int key, string value);
bool search(int key);
int isFull(void);
int isEmpty(void);
int getDepth(void);
int incDepth(void);
int decDepth(void);
std::map<int, string> copy(void);
void clear(void);
void display(void);
};

class Directory {
int global_depth, bucket_size;
std::vector<Bucket*> buckets;
int hash(int n);
int pairIndex(int bucket_no, int depth);
void grow(void);
void shrink(void);
void split(int bucket_no);
void merge(int bucket_no);
string bucket_id(int n);
public:
Directory(int depth, int bucket_size);
void insert(int key,string value,bool reinserted);
void remove(int key,int mode);
void update(int key, string value);
void search(int key);
void display(bool duplicates);
};


void menu();


/* Main function */
int main(){
int bucket_size,initial_global_depth;
cin>>initial_global_depth;
cin>>bucket_size;
Directory d(initial_global_depth,bucket_size);
int mode;
int j;
string x = "x";
while(true){
cin>>mode;
if(mode == 6){
break;
}
switch(mode){
case 2:
cin>>j;

d.insert(j,x,0);
break;
case 3:
cin>>j;
d.search(j);
break;
case 4:
cin>>j;
d.remove(j,0);
break;
case 5:
d.display(0);
break;
}

}

return (0);
}



/* Directory class functions */

Directory::Directory(int depth, int bucket_size)
{
this->global_depth = depth;
this->bucket_size = bucket_size;
for(int i = 0 ; i < 1<<depth ; i++ )
{
buckets.push_back(new Bucket(depth,bucket_size));
}
}

int Directory::hash(int n)
{
return n&((1<<global_depth)-1);
}

int Directory::pairIndex(int bucket_no, int depth)
{
return bucket_no^(1<<(depth-1));
}

void Directory::grow(void)
{
for(int i = 0 ; i < 1<<global_depth ; i++ )
buckets.push_back(buckets[i]);
global_depth++;
}

void Directory::shrink(void)
{
int i,flag=1;
for( i=0 ; i<buckets.size() ; i++ )
{
if(buckets[i]->getDepth()==global_depth)
{
flag=0;
return;
}
}
global_depth--;
for(i = 0 ; i < 1<<global_depth ; i++ )
buckets.pop_back();
}

void Directory::split(int bucket_no)
{
int local_depth,pair_index,index_diff,dir_size,i;
map<int, string> temp;
map<int, string>::iterator it;

local_depth = buckets[bucket_no]->incDepth();
if(local_depth>global_depth)
grow();
pair_index = pairIndex(bucket_no,local_depth);
buckets[pair_index] = new Bucket(local_depth,bucket_size);
temp = buckets[bucket_no]->copy();
buckets[bucket_no]->clear();
index_diff = 1<<local_depth;
dir_size = 1<<global_depth;
for( i=pair_index-index_diff ; i>=0 ; i-=index_diff )
buckets[i] = buckets[pair_index];
for( i=pair_index+index_diff ; i<dir_size ; i+=index_diff )
buckets[i] = buckets[pair_index];
for(it=temp.begin();it!=temp.end();it++)
insert((*it).first,(*it).second,1);
}

void Directory::merge(int bucket_no)
{
int local_depth,pair_index,index_diff,dir_size,i;

local_depth = buckets[bucket_no]->getDepth();
pair_index = pairIndex(bucket_no,local_depth);
index_diff = 1<<local_depth;
dir_size = 1<<global_depth;

if( buckets[pair_index]->getDepth() == local_depth )
{
buckets[pair_index]->decDepth();
delete(buckets[bucket_no]);
buckets[bucket_no] = buckets[pair_index];
for( i=bucket_no-index_diff ; i>=0 ; i-=index_diff )
buckets[i] = buckets[pair_index];
for( i=bucket_no+index_diff ; i<dir_size ; i+=index_diff )
buckets[i] = buckets[pair_index];
}
}

string Directory::bucket_id(int n)
{
int d;
string s;
d = buckets[n]->getDepth();
s = "";
while(n>0 && d>0)
{
s = (n%2==0?"0":"1")+s;
n/=2;
d--;
}
while(d>0)
{
s = "0"+s;
d--;
}
return s;
}

void Directory::insert(int key,string value,bool reinserted)
{
int bucket_no = hash(key);
int status = buckets[bucket_no]->insert(key,value);
if(status==1)
{
if(!reinserted)
return;
else
return;
}
else if(status==0)
{
split(bucket_no);
insert(key,value,reinserted);
}
else
{
return;
}
}

void Directory::remove(int key,int mode)
{
int bucket_no = hash(key);
if(buckets[bucket_no]->remove(key))
return;
if(mode>0)
{
if(buckets[bucket_no]->isEmpty() && buckets[bucket_no]->getDepth()>1)
merge(bucket_no);
}
if(mode>1)
{
shrink();
}
}

void Directory::update(int key, string value)
{
int bucket_no = hash(key);
buckets[bucket_no]->update(key,value);
}

void Directory::search(int key)
{
int bucket_no = hash(key);
cout<<"Searching key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
buckets[bucket_no]->search(key);
}

void Directory::display(bool duplicates)
{
int i,j,d;
string s;
std::set<string> shown;
int cnt=0;

for(i=0;i<buckets.size();i++)
{
d = buckets[i]->getDepth();
s = bucket_id(i);
if(duplicates || shown.find(s)==shown.end())
{
shown.insert(s);
cnt++;
}
}
shown.clear();
cout<<global_depth<<endl;
cout<<cnt<<endl;

for(i=0;i<buckets.size();i++)
{
d = buckets[i]->getDepth();
s = bucket_id(i);
if(duplicates || shown.find(s)==shown.end())
{
shown.insert(s);
buckets[i]->display();
cout<<" "<<d<<endl;
}
}
}



/* Bucket class functions */

Bucket::Bucket(int depth, int size)
{
this->depth = depth;
this->size = size;
}

int Bucket::insert(int key,string value){
std::map<int,string>::iterator it;
it = values.find(key);
if(it!=values.end()){
return -1;
}else if(isFull()){
return 0;
}else{
values[key]=value;
}
}

int Bucket::remove(int key){
std::map<int,string>::iterator it;
it = values.find(key);
if(it!=values.end()){
values.erase(it);
return 1;
}else{
return 0;
}
}

int Bucket::update(int key,string value){
std::map<int,string>::iterator it;
it = values.find(key);
if(it!=values.end()){
values[key]=value;
return 1;
}else{
return 0;
}
}

bool Bucket::search(int key){
std::map<int,string>::iterator it;
it = values.find(key);
if(it!=values.end()){
return true;
}
else{
return false;
}
}

int Bucket::isFull(void){
if(values.size()==size)
return 1;
else
return 0;
}

int Bucket::isEmpty(void){
if(values.size()==0)
return 1;
else
return 0;
}

int Bucket::getDepth(void){
return depth;
}

int Bucket::incDepth(void){
depth++;
return depth;
}

int Bucket::decDepth(void){
depth--;
return depth;
}

std::map<int, string> Bucket::copy(void){
std::map<int, string> temp(values.begin(),values.end());
return temp;
}

void Bucket::clear(void){
values.clear();
}

void Bucket::display(){
std::map<int,string>::iterator it;
cout<<values.size()<<" ";
}
