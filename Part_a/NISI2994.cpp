#include <algorithm>
#include <fstream>
#include <iostream>
#include <istream>
#include <new>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <valarray>
#include <vector>

using namespace std;
class Node { 
public: 
    char data; 
    Node* left; 
    Node* right; 
    bool nullable = 0;
    set<int> firstpos;
    set<int> lastpos;
    int id = -1;
  
    // Default constructor 
    Node(char data) 
    { 
        this->data = data; 
        left = NULL;
        right = NULL;
    } 
  
    // Parameterised Constructor 
    Node(char data, Node* left, Node* right) 
    { 
        this->data = data; 
        this->left = left; 
        this->right = right; 
    } 
};
vector<set<int>> hash_fp(50);
vector<char> index_char (50); 
int stored_dfa[10][15][4]; 
int final_id;
int id_count = 1;
int inserting_dfa_index = 0;
Node* buildNode(char op, Node* left, Node* right) {
    return new Node(op, left, right);
  }
bool compare(char op1, char op2) {
    if (op1 == '(' || op1 == ')')
      return false;
    return op1 == '*' || op1 == '|' || op2 == '+' || op2 == '.';
  }
char pop(stack<char>& ops) {
    const char op = ops.top();
    ops.pop();
    return op;
  }
Node* pop(stack<Node*>& nodes) {
    Node* node = nodes.top();
    nodes.pop();
    return node;
  }
Node* expTree(string s) {
    stack<Node*> nodes;  
    stack<char> ops;     

    for (const char c : s)
      if (c == 'a' || c == 'b' || c=='z' || c=='#') {
        nodes.push(new Node(c));
      } else if (c == '(') {
        ops.push(c);
      } else if (c == ')') {
        while (ops.top() != '(')
        {
            char tp = pop(ops);
            if(tp == '+')
            {
                Node* z = pop(nodes);
                Node* a = pop(nodes);
                Node* new_a = new Node(*a);
                Node* star = buildNode('*',new_a,z);
                Node* dot = buildNode('.', a,star);                       //Handling '+'
                nodes.push(dot);
            }
            else if(tp == '?')
            {
                Node* z = pop(nodes);
                Node* a = pop(nodes);
                Node* line = buildNode('|',a,z);                      //Handling '?'
                nodes.push(line);
            }
            else
            {
                Node* rt = pop(nodes);
                Node* lt = pop(nodes);
                nodes.push(buildNode(tp, lt, rt));
            }
        }
        ops.pop();  // Remove '('
      } 
      
      
      
      else if (c == '+' || c == '|' || c == '*' || c == '.' || c=='?') {
        while (!ops.empty() && compare(ops.top(), c))
        {
            char tp = pop(ops);
            if(tp == '+')
            {
                Node* z = pop(nodes);
                Node* a = pop(nodes);
                Node* new_a = new Node(*a);
                Node* star = buildNode('*',new_a,z);
                Node* dot = buildNode('.', a,star);                       //Handling '+'
                nodes.push(dot);
            }
            else if(tp == '?')
            {
                Node* z = pop(nodes);
                Node* a = pop(nodes);
                Node* line = buildNode('|',a,z);                      //Handling '?'
                nodes.push(line);
            }
            else
            {
                Node* rt = pop(nodes);
                Node* lt = pop(nodes);
                nodes.push(buildNode(tp, lt, rt));
            }
        }
        ops.push(c);
      }


    while (!ops.empty())
      {    
            char tp = pop(ops);
            if(tp == '+')
            {
                Node* z = pop(nodes);
                Node* a = pop(nodes);
                Node* new_a = new Node(*a);
                Node* star = buildNode('*',new_a,z);
                Node* dot = buildNode('.', a,star);                       //Handling '+'
                nodes.push(dot);
            }
            else if(tp == '?')
            {
                Node* z = pop(nodes);
                Node* a = pop(nodes);
                Node* line = buildNode('|',a,z);                      //Handling '?'
                nodes.push(line);
            }
            else
            {
                Node* rt = pop(nodes);
                Node* lt = pop(nodes);
                nodes.push(buildNode(tp, lt, rt));
            }
            }

    return nodes.top();
  }
string dotzAdder(string x)
{
    string s = "";
    for(int i = 0;i<x.size()-1;i++)
    {
        if(x[i] == ')' && x[i+1] == '(')
        {
            s+= ").";
        }
        else if(x[i] == '*')
        {
            s+="*z";
        }
        else if(x[i] == '+')
        {
            s+="+z";
        }
        else if(x[i] == '?')
        {
            s+="?z";
        }
        else
        s+=x[i];
    }
    s+=".(#)";
    s+=x[x.size()-1];
    return s;
}
void dfs(Node* node){
    if(node->left != nullptr) dfs(node->left);
    if(node->right != nullptr) dfs(node->right);
    if (node->data == '*')
    {
        node->nullable = true;
        if (node->left != nullptr)
        {
            for (auto x : node->left->firstpos)
            {
                node->firstpos.insert(x);
            }
            for (auto x : node->left->lastpos)
            {
                node->lastpos.insert(x);
            }
        }
    }
    else if (node->data == '.')
    {
        if (node->left != nullptr && node->right != nullptr)
        {
            if (node->left->nullable && node->right->nullable)
                node->nullable = true;
        }
        if (node->left != nullptr)
        {
            for (auto x : node->left->firstpos)
            {
                node->firstpos.insert(x);
            }
            if (node->left->nullable == true)
            {
                for (auto x : node->right->firstpos)
                {
                    node->firstpos.insert(x);
                }
            }
        }
        if (node->right != nullptr)
        {
            if (node->right->data != 'z')
                for (auto x : node->right->lastpos)
                {
                    node->lastpos.insert(x);
                }
            if (node->right->nullable == true)
            {
                for (auto x : node->left->lastpos)
                {
                    node->lastpos.insert(x);
                }
            }
        }
    }
    else if (node->data == '|')
    {
        if (node->left != nullptr && node->right != nullptr)
        {
            if (node->left->nullable || node->right->nullable)
                node->nullable = true;
        }
        if (node->left != nullptr)
        {
            for (auto x : node->left->firstpos)
            {
                node->firstpos.insert(x);
            }
            for (auto x : node->left->lastpos)
            {
                node->lastpos.insert(x);
            }
        }
        if (node->right != nullptr)
        {
            for (auto x : node->right->lastpos)
            {
                node->lastpos.insert(x);
            }
            for (auto x : node->right->firstpos)
            {
                node->firstpos.insert(x);
            }
        }
    }
    else if (node->data == 'z')
    {
        node->nullable = true;
    }
    else if (node->left == nullptr && node->right == nullptr && node->data != 'z')
    { 
        node->id= id_count++;
        index_char[node->id] = node->data;
        node->firstpos.insert(node->id);
        node->nullable = false;
        node->lastpos.insert(node->id);
    }
    if(node->data == '#')
    {
        final_id=(node->id);
    }
}
void follow_pos(Node * node)
{
    if(node->left != nullptr) follow_pos(node->left);
    if(node->right != nullptr) follow_pos(node->right);
    if(node->data == '.')
    {
        for(auto left_ch : node->left->lastpos){
            for(auto right_ch : node->right->firstpos){
                hash_fp[left_ch].insert(right_ch);
            }
        }
    }
    else if(node->data == '*')
    {
        for(auto last_p : node->lastpos){
            for(auto first_p : node->firstpos){
                hash_fp[last_p].insert(first_p);
            }
        }
    }
}
int unique_num(set<int> s)
{
    int ans = 0;
    for(auto x: s)
    {
        ans+=pow(2,x);
    }
    return ans;
}
set<int> extract_nums(int num)
{
    set<int> list;
    for(int i =0; i<32; i++)
    {
        if(num & (1<<i)) list.insert(i); 
    }
    return list;
}
void find_dfa(Node* node, int regex_number)
{
    queue<int> pp;
    set<int> cur_state_list;
    pp.push(unique_num(node->firstpos));
    while(!pp.empty())
    {
        int cur = pp.front();
        pp.pop();
        if(cur_state_list.find(cur)!=cur_state_list.end()) continue;
        cur_state_list.insert(cur);
        set<int> list = extract_nums(cur);
        set<int> next_a;
        set<int> next_b;
        for(auto x: list)
        {
            if(index_char[x]=='a')
            {
                for(auto y: hash_fp[x])
                {
                    next_a.insert(y);
                }
            }
        }
        for(auto x: list)
        {
            if(index_char[x]=='b')
            {
                for(auto y: hash_fp[x])
                {
                    next_b.insert(y);
                }
            }
        }
        int next_a_num = unique_num(next_a);
        int next_b_num = unique_num(next_b);
        if(cur_state_list.find(next_a_num)== cur_state_list.end()) pp.push(next_a_num);
        if(cur_state_list.find(next_b_num)== cur_state_list.end()) pp.push(next_b_num);
        stored_dfa[regex_number][inserting_dfa_index][0] = cur;
        stored_dfa[regex_number][inserting_dfa_index][1] = next_a_num;
        stored_dfa[regex_number][inserting_dfa_index][2] = next_b_num;
        inserting_dfa_index++;
    }
    for(int i=0; i<inserting_dfa_index; i++)
    {
        if(stored_dfa[regex_number][i][0]>=pow(2,final_id)) stored_dfa[regex_number][i][3]++;
    }
}
pair<bool,int> traverse_string(string s, int start_pos, int regex_number)
{
    int cur = 0;
    pair<bool,int> ans; ans.first = false;
    ans.second = -1;
    for(int i=start_pos; i<s.size(); i++)
    {
        int val = s[i] - 'a' + 1; 
        int req_num = stored_dfa[regex_number][cur][val];
        int next_index = 0;
        for(int i=0; i<15; i++)
        {
            if(stored_dfa[regex_number][i][0]==req_num) 
            {
                next_index = i;
                break;
            }
        }
        cur = next_index; 
        if(stored_dfa[regex_number][cur][3] == 1)
        {
            ans.first = true;
            ans.second = i - start_pos +1;
        }
        if(cur==-1) {break; }
    }
    return ans;
}
int main(){
    vector<string> v; // everything
    vector<Node*> p; // node ke roots for every re;
    string myText;
    ifstream inputfile("input.txt");
    while (getline(inputfile, myText))
    {
        v.push_back(myText);
    }
    inputfile.close();
    string w = v[0]; 
    for(int i = 1;i<v.size();i++)
    {
        v[i] = dotzAdder(v[i]);
        p.push_back(expTree(v[i]));
    }
    for(int i = 0;i<p.size();i++)
    {
        dfs(p[i]);
        follow_pos(p[i]);
        find_dfa(p[i], i);
        id_count=1;
        inserting_dfa_index = 0;
        for(int j=0; j<50; j++)
        {
            hash_fp[j].clear();
        }
    }
    for(int i=0; i<2; i++)
    {
        for(int j=0; j<6; j++)
        {
            for(int k =0; k<4; k++)
            {
                if(stored_dfa[i][j][k]==0) stored_dfa[i][j][k] = -1;
            }
        }
    }
    int string_index = 0;
    ofstream opfile("output.txt");
    while(string_index<w.size())
    {
        int max_length = 0;
        int max_wala_i = 0;
        for(int i=0; i<p.size(); i++)
        {
            pair<bool,int> bool_length = traverse_string(w,string_index,i);
            if(bool_length.first)
            {
                if (bool_length.second > max_length)
                {
                    max_length = bool_length.second;
                    max_wala_i = i + 1;
                }
            } 
        }
        if(max_wala_i==0) max_length++;
        opfile<<"<";
        for(int i=string_index; i<string_index+max_length; i++)
        {
            opfile<<w[i];
        }
        opfile<<","<<max_wala_i<<">";
        string_index+=max_length;
    }
    opfile.close();
}