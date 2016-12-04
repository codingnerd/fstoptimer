#include <Arduino.h>
#include "Fraction.h"
int gcd(int n, int m)
{
  int gcd, remainder;
 
  while (n != 0)
    {
      remainder = m % n;
      m = n;
      n = remainder;
    }
 
  gcd = m;
 
  return gcd;
}//end gcd function

void frac2string(struct frac a, String* ans) {
  *ans = String(a.num)+"/"+String(a.den);
}

void redfrac2string(struct redfrac a, String* ans) {
  if (a.intpart < 0) {
    *ans = "-";
    a.intpart = -a.intpart;
  }

  if (a.num < 0) {
    *ans = "-";
    a.num = -a.num;
  }

  if (a.den < 0) {
    *ans = "-";
    a.den = -a.den;
  }

  if (a.intpart == 0) {
    if (0==a.num) {
      *ans += String("0");    
    } else {
      *ans += String(a.num)+"/"+String(a.den);    
    } 
  } else {
    if (1==a.den) {    
      *ans += String(a.intpart);    
    } else {
      *ans += String(a.intpart)+"$"+String(a.num)+"/"+String(a.den);    
    }
  }
}

struct frac addfrac(struct frac a, struct frac b) {
  struct frac ans;
  ans.num = (a.num * b.den) + (b.num * a.den);
  ans.den = a.den * b.den;
  
  int mygcd = gcd(ans.den, ans.num);  
  
  ans.num = ans.num / mygcd;
  ans.den = ans.den / mygcd;
   
  return ans;
};

struct frac subtractfrac(struct frac a, struct frac b) {
  struct frac ans;
  ans.num = (a.num * b.den) - (b.num * a.den);
  ans.den = a.den * b.den;
  
  int mygcd = gcd(ans.den, ans.num);  
  
  ans.num = ans.num / mygcd;
  ans.den = ans.den / mygcd;
   
  return ans;
};

struct redfrac reducefrac(struct frac a) {
  redfrac ans;
       
    ans.intpart = a.num / a.den;   
    ans.den = a.den; 
    ans.num = a.num % a.den;

    return ans;
}
