module func(output F, input x0, input x1, input x2, input x3, input x4, input x5, input x6, input x7, input x8, input x9);

  wire nx0;
  wire nx1;
  wire nx2;
  wire nx3;
  wire nx4;
  wire nx5;
  wire nx6;
  wire nx7;
  wire nx8;
  wire nx9;

  not(nx0, x0);
  not(nx1, x1);
  not(nx2, x2);
  not(nx3, x3);
  not(nx4, x4);
  not(nx5, x5);
  not(nx6, x6);
  not(nx7, x7);
  not(nx8, x8);
  not(nx9, x9);

  wire w0;
  and(w0, nx0, nx1, nx2, nx3, nx4, nx5, nx7, x8, nx9);
  wire w1;
  and(w1, nx0, nx1, nx2, nx3, nx4, nx5, nx6, nx8, x9);
  wire w2;
  and(w2, nx0, nx1, nx2, nx3, nx4, nx5, nx6, nx7, nx8, x9);
  wire w3;
  and(w3, nx0, nx1, nx2, nx3, nx4, nx5, nx6, nx7, x8, nx9);
  wire w4;
  and(w4, nx0, nx1, nx2, nx3, nx4, nx5, nx6, x7, nx8, x9);
  wire w5;
  and(w5, nx0, nx1, nx2, nx3, nx4, nx5, x6, nx7, nx9);
  wire w6;
  and(w6, nx0, nx1, nx2, nx3, nx4, nx5, x6, nx7, nx8, nx9);
  wire w7;
  and(w7, nx0, nx1, nx2, nx3, nx4, nx5, x6, nx7, x8, nx9);
  wire w8;
  and(w8, nx0, nx1, nx2, nx3, nx4, nx5, x6, x7, x8, x9);
  wire w9;
  and(w9, nx0, nx1, nx2, nx3, nx4, x5, nx6, x7, nx8, nx9);
  wire w10;
  and(w10, nx0, nx1, nx2, nx3, nx4, x5, x6, nx7, nx8, x9);
  wire w11;
  and(w11, nx0, nx1, nx2, nx3, nx4, x5, x6, x7, x8, nx9);
  wire w12;
  and(w12, nx0, nx1, nx2, nx3, x4, nx5, nx6, nx7, x8, x9);

  or(F, w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12);
endmodule
