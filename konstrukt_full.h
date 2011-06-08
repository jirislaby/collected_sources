#ifndef KONSTRUKT_FULL_H
#define KONSTRUKT_FULL_H

#include<iostream>

using namespace std;

struct t1
{ 
  t1();
  ~t1();
};

struct t2: virtual t1
{
  t2();
  ~t2();
};

struct t3: virtual t1,t2
{
  t3();
  virtual void abstract() = 0;
  virtual ~t3();
};

struct t4:t3
{
  t4();
  virtual void abstract() = 0;
  virtual ~t4();
};

struct t5:t4
{
  t5();
  void abstract(){};
  virtual ~t5();
};

#endif // KONSTRUKT_FULL_H
