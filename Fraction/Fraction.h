typedef struct frac {
  int num;
  int den;
};

typedef struct redfrac {
  int num;
  int den;
  int intpart;
};

int gcd(int n, int m);

void frac2string(struct frac a, String* ans);

void redfrac2string(struct redfrac a, String* ans);

struct frac addfrac(struct frac a, struct frac b);

struct frac subtractfrac(struct frac a, struct frac b);

struct redfrac reducefrac(struct frac a);
