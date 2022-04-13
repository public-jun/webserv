#ifndef HOGE_HPP
#define HOGE_HPP

class hoge
{
private:
	int num_;
public:
	hoge(int i) : num_(i) {}
	~hoge(){}

	int get_num()
	{
		return num_;
	}
};


#endif