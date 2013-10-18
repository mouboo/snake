struct colour_struct{
  int r;
  int g;
  int b;
};
typedef struct colour_struct colour;


struct coord_struct{
  int x;
  int y;
  struct coord_struct *next_ptr; 
};
typedef struct coord_struct coord;


struct {
  int x;
  int y;
  int growBy;
} food; 
