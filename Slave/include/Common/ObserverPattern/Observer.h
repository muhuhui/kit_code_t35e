#pragma once
#include<vector>
#include<iostream>
#include<string>
using namespace std;
class Subject;

class Observer{                    //观察者抽象类
public:
	virtual void update(Subject *base) = 0;
	virtual void showTips(Subject *base, string strTip) = 0;
protected:
	Subject * _subject;
};

class Subject{                    //目标抽象类
public:
	void regiObserver(Observer *obs){
		_observer.push_back(obs);
		cout << "已注册" << endl;
	}
	void deleObserver(Observer *obs){
		_observer.pop_back();
	}
	void notify(){                //更新所有的观察者
		vector<Observer *>::iterator it;
		for (it = _observer.begin(); it != _observer.end(); it++)
			(*it)->update(this);
	}
	void showTips(string strTip)
	{
		vector<Observer *>::iterator it;
		for (it = _observer.begin(); it != _observer.end(); it++)
			(*it)->showTips(this, strTip);
	}
private:
	vector<Observer *> _observer;        //观察者容器
};

