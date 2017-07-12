#include <iostream> 
#include <queue> 
#include <string>
#include <tuple>
#include <list>
using namespace std; 
struct pose{

};
class edge;
struct vertex
{
	friend class edge;
	public:
		pose x;
		list<edge*> adjacent_edge; 
		bool operator ==(const vertex &v)const{
			return true;
		}
};
class edge
{
public:
	void setlistpi(list<double> listPi){
		listpi=listPi;
	}
	bool Nullhypothesisexists(){
		double x;
		for(list<double>::iterator i=listpi.begin();i!=listpi.end();i++)
		{
			x+=*i;
		}
		return x<1;
	}
	vertex start(){
		return vi;
	}
	list<vertex> listvrequest(){
		return listv;
	}
	list<list<double> > crequest(){
		return listc;
	}
	int edgeweight(){
		int x = listpi.size();
		if( this->Nullhypothesisexists() )x+=1;
		Edgeweight=x;
		return x;
	}
	bool operator <(const edge &e) const{
		//return this->edgeweight()<e.edgeweight();
		return (Edgeweight>e.Edgeweight);
	}
private:
	vertex vi;
	int N;
	list<vertex> listv;
	list<double> listpi;
	list<list<double>> listc;
	int Edgeweight;
};
class traversalstate
{
	public:
		tuple<list<pose>,list<vertex>,list<edge>,priority_queue<edge> > Tuple;
		void set(list<pose> X,list<vertex> V,list<edge> E,priority_queue<edge> P){
			Tuple=make_tuple(X,V,E,P);
		}
};
list<traversalstate> ExpandMultimodal(
		list<traversalstate> &T,
		traversalstate &Current1,
		vertex &v_current,
		vertex &v_next,
		 list<double> &c )
{
	for (list<double>::iterator m=c.begin(); m!=c.end() ;m++){
		traversalstate Current2 = Current1;
		pose x=v_current.x;
		list<pose> X = get<0>(Current1.Tuple);
		list<vertex> V = get<1>(Current1.Tuple);
		list<edge> E = get<2>(Current1.Tuple);
		priority_queue<edge> P = get<3>(Current1.Tuple);
		X.push_back(x);
		V.push_back(v_next);
		for (list<edge*>::iterator i=v_next.adjacent_edge.begin();i!=v_next.adjacent_edge.end();i++)
		{
			P.push(**i);
		}
		traversalstate package;
		package.set(X,V,E,P);
		T.push_back(package);
	}
	return T;
}
bool check(traversalstate &temp,int size){
	return (!(get<3>(temp.Tuple).empty()) and get<1>(temp.Tuple).size() < size)?true:false ;
}
void topNforT(list<traversalstate> &temp,int max){}
int main() 
{
	int N;
	list<traversalstate> T;
	traversalstate t;
	list<pose> X;
	list<vertex> V;
	list<edge> E;
	priority_queue<edge> P;
	pose x;
	vertex v;
	X.push_back(x);V.push_back(v);
	for (list<edge*>::iterator i=v.adjacent_edge.begin();i!=v.adjacent_edge.end();i++)
	{
		P.push(**i);
	}
	t.set(X,V,E,P);
	T.push_back(t);
	int counter=0;
	while (counter<=1+T.size() ){
		for (list<traversalstate>::iterator pointer=T.begin();pointer!=T.end();pointer++){
			if (check(*pointer,N)){//to change the N to |V|
				counter=0;
				edge ee=P.top();vertex vv=ee.start();
				list<list<double> > c_countainer=ee.crequest();
				if (v==vv){
					list<vertex> V_container=ee.listvrequest();
					list<list<double> >::iterator zz2=c_countainer.begin();
					for (list<vertex>::iterator zz1 = V_container.begin();zz1!=V_container.end();zz1++)
					{
						ExpandMultimodal(T,t,v,*zz1,*zz2);
						zz2++;
					}
				}else{
					list<vertex> V_container=ee.listvrequest();
					list<list<double> >::iterator zz2=c_countainer.begin();
					for (list<vertex>::iterator zz1 = V_container.begin();zz1!=V_container.end();zz1++)
					{
						if (*zz1==v)ExpandMultimodal(T,t,*zz1,vv,*zz2);
						zz2++;
					}
				}
			}else{
				counter+=1;
			}

		}
		if (T.size()>N){
			topNforT(T,N);
		}
	}
	return 1;
	
}