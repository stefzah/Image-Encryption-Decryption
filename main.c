#include < stdio.h > 
#include < stdlib.h >
  typedef struct {
    unsigned char r, g, b;
  }
pixel;
unsigned int * XORSHIFT32(int W, int H, unsigned int seed) {
  unsigned int * R, i;
  R = malloc((2 * W * H - 1) * sizeof(int));
  unsigned int x = seed;
  R[0] = x;
  for (i = 1; i <= 2 * W * H - 1; i++) {
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    R[i] = x;
  }
  return R;
}
pixel ** CitireIMG(char * fisier) {
  int i, W, H, W2, H2, j;
  pixel ** P;
  unsigned char x;
  FILE * fin;
  fin = fopen(fisier, "rb");
  fseek(fin, 18, SEEK_SET);
  fread( & W, 4, 1, fin);
  fread( & H, 4, 1, fin);
  P = malloc(sizeof(int) * H);
  for (i = 0; i < H; i++)
    P[i] = malloc(sizeof(pixel) * W);
  fseek(fin, 54, SEEK_SET);
  for (i = H - 1; i >= 0; i--) {
    for (j = 0; j < W; j++) {
      fread( & x, 1, 1, fin);
      P[i][j].b = x;
      fread( & x, 1, 1, fin);
      P[i][j].g = x;
      fread( & x, 1, 1, fin);
      P[i][j].r = x;
    }
    j = W * 3;
    while (j % 4 != 0) {
      j++;
      fread( & x, 1, 1, fin);
    }

  }
  fclose(fin);
  return P;
}
pixel * CitireLiniarIMG(char * fisier) {
  int i, W, H, W2, H2, j, ct = 0;
  pixel ** P, * Pl;
  unsigned char x;
  FILE * fin;
  fin = fopen(fisier, "rb");
  fseek(fin, 18, SEEK_SET);
  fread( & W, 4, 1, fin);
  fread( & H, 4, 1, fin);
  P = CitireIMG(fisier);
  Pl = malloc(sizeof(pixel) * W * H);
  for (i = 0; i < H; i++) {
    for (j = 0; j < W; j++) {
      Pl[ct] = P[i][j];
      ct++;
    }
  }
  fclose(fin);
  return Pl;
}
void StocareLiniarIMG(char * fisier) {
  int i, W, H, W2, H2, j, x;
  pixel * P;
  char c;
  FILE * fin, * fout;
  fin = fopen(fisier, "rb");
  fout = fopen("Imagine_stocata_liniar.bmp", "wb");
  for (i = 0; i < 54; i++) {
    fread( & x, 1, 1, fin);
    fwrite( & x, 1, 1, fout);
  }
  fseek(fin, 18, SEEK_SET);
  fread( & W, 4, 1, fin);
  fread( & H, 4, 1, fin);
  P = CitireLiniarIMG(fisier);
  for (i = H - 1; i >= 0; i--) {
    for (j = 0; j < W; j++) {
      fwrite( & P[i * W + j].b, 1, 1, fout);
      fwrite( & P[i * W + j].g, 1, 1, fout);
      fwrite( & P[i * W + j].r, 1, 1, fout);
    }
    j = 3 * W;
    while (j % 4 != 0) {
      j++;
      c = 0;
      fwrite( & c, 1, 1, fout);
    }
  }
  fclose(fin);
}
void CriptareIMG(char * fisier_intrare, char * fisier_iesire, char * fisier_cheie) {
  unsigned int * R, * Perm, aux, W, H, j, x, SV, seed;
  int i, k;
  unsigned char R1, R2, R3, c, SV1, SV2, SV3;
  FILE * fin, * finc, * fout;
  pixel * P, * C, * P2;
  fin = fopen(fisier_intrare, "rb");
  finc = fopen(fisier_cheie, "r");
  fout = fopen(fisier_iesire, "wb");
  fscanf(finc, "%u%u", & seed, & SV);
  for (i = 0; i < 54; i++) {
    fread( & x, 1, 1, fin);
    fwrite( & x, 1, 1, fout);
  }
  fseek(fin, 18, SEEK_SET);
  fread( & W, 4, 1, fin);
  fread( & H, 4, 1, fin);
  R = XORSHIFT32(W, H, seed);
  Perm = malloc(W * H * sizeof(int));
  for (i = 0; i < W * H; i++)
    Perm[i] = i;
  for (i = W * H - 1; i >= 1; i--) {
    k = W * H - i;
    j = R[k] % (i + 1);
    aux = Perm[i];
    Perm[i] = Perm[j];
    Perm[j] = aux;
  }
  P = CitireLiniarIMG(fisier_intrare);
  P2 = malloc(W * H * sizeof(int));
  C = malloc(W * H * sizeof(int));
  for (i = 0; i < W * H; i++)
    P2[Perm[i]] = P[i];
  R1 = ((R[W * H] << 8) >> 24);
  R2 = ((R[W * H] << 16) >> 24);
  R3 = ((R[W * H] << 24) >> 24);
  SV1 = ((SV << 8) >> 24);
  SV2 = ((SV << 16) >> 24);
  SV3 = ((SV << 24) >> 24);
  C[0].r = SV1 ^ P2[0].r ^ R1;
  C[0].g = SV2 ^ P2[0].g ^ R2;
  C[0].b = SV3 ^ P2[0].b ^ R3;
  for (i = 1; i < W * H; i++) {
    R1 = ((R[i + W * H] << 8) >> 24);
    R2 = ((R[i + W * H] << 16) >> 24);
    R3 = ((R[i + W * H] << 24) >> 24);
    C[i].r = C[i - 1].r ^ P2[i].r ^ R1;
    C[i].g = C[i - 1].g ^ P2[i].g ^ R2;
    C[i].b = C[i - 1].b ^ P2[i].b ^ R3;
  }
  for (i = H - 1; i >= 0; i--) {
    for (j = 0; j < W; j++) {
      fwrite( & C[i * W + j].b, 1, 1, fout);
      fwrite( & C[i * W + j].g, 1, 1, fout);
      fwrite( & C[i * W + j].r, 1, 1, fout);
    }
    j = 3 * W;
    while (j % 4 != 0) {
      j++;
      c = 0;
      fwrite( & c, 1, 1, fout);
    }
  }
  fclose(fin);
  fclose(finc);
  fclose(fout);
}
void DecriptareIMG(char * fisier_intrare, char * fisier_iesire, char * fisier_cheie) {
  unsigned int * R, * Perm, aux, W, H, j, x, SV, seed, * Perm2;
  int i, k;
  unsigned char R1, R2, R3, c, SV1, SV2, SV3;
  FILE * fin, * finc, * fout;
  pixel * C, * C2, * D;
  fin = fopen(fisier_intrare, "rb");
  finc = fopen(fisier_cheie, "r");
  fout = fopen(fisier_iesire, "wb");
  fscanf(finc, "%u%u", & seed, & SV);
  for (i = 0; i < 54; i++) {
    fread( & x, 1, 1, fin);
    fwrite( & x, 1, 1, fout);
  }
  fseek(fin, 18, SEEK_SET);
  fread( & W, 4, 1, fin);
  fread( & H, 4, 1, fin);
  R = XORSHIFT32(W, H, seed);
  Perm = malloc(W * H * sizeof(int));
  for (i = 0; i < W * H; i++)
    Perm[i] = i;
  for (i = W * H - 1; i >= 1; i--) {
    k = W * H - i;
    j = R[k] % (i + 1);
    aux = Perm[i];
    Perm[i] = Perm[j];
    Perm[j] = aux;
  }
  Perm2 = malloc(W * H * sizeof(int));
  for (i = 0; i < W * H; i++)
    Perm2[Perm[i]] = i;
  C = CitireLiniarIMG(fisier_intrare);
  C2 = malloc(W * H * sizeof(int));
  D = malloc(W * H * sizeof(int));
  R1 = ((R[W * H] << 8) >> 24);
  R2 = ((R[W * H] << 16) >> 24);
  R3 = ((R[W * H] << 24) >> 24);
  SV1 = ((SV << 8) >> 24);
  SV2 = ((SV << 16) >> 24);
  SV3 = ((SV << 24) >> 24);
  C2[0].r = SV1 ^ C[0].r ^ R1;
  C2[0].g = SV2 ^ C[0].g ^ R2;
  C2[0].b = SV3 ^ C[0].b ^ R3;
  for (i = 1; i < W * H; i++) {
    R1 = ((R[i + W * H] << 8) >> 24);
    R2 = ((R[i + W * H] << 16) >> 24);
    R3 = ((R[i + W * H] << 24) >> 24);
    C2[i].r = C[i - 1].r ^ C[i].r ^ R1;
    C2[i].g = C[i - 1].g ^ C[i].g ^ R2;
    C2[i].b = C[i - 1].b ^ C[i].b ^ R3;
  }

  for (i = 0; i < W * H; i++)
    D[Perm2[i]] = C2[i];
  for (i = H - 1; i >= 0; i--) {
    for (j = 0; j < W; j++) {
      fwrite( & D[i * W + j].b, 1, 1, fout);
      fwrite( & D[i * W + j].g, 1, 1, fout);
      fwrite( & D[i * W + j].r, 1, 1, fout);
    }
    j = W * 3;
    while (j % 4 != 0) {
      j++;
      c = 0;
      fwrite( & c, 1, 1, fout);
    }
  }
  fclose(fin);
  fclose(finc);
  fclose(fout);
}
void ChiPatrat(char * fisier) {
  FILE * fin;
  fin = fopen(fisier, "rb");
  unsigned int i, j, H, W;
  double f[256][3], fp, Xi[3];
  unsigned char x;
  fseek(fin, 18, SEEK_SET);
  fread( & W, 4, 1, fin);
  fread( & H, 4, 1, fin);
  fseek(fin, 54, SEEK_SET);
  fp = (double)(W * H) / 256;
  for (i = 0; i < 256; i++)
    for (j = 0; j < 3; j++)
      f[i][j] = 0;
  Xi[0] = Xi[1] = Xi[2] = 0;
  while (fread( & x, 1, 1, fin)) {
    f[x][j]++;
    j = (j + 1) % 3;
  }
  for (i = 0; i < 256; i++) {
    Xi[0] += (f[i][0] - fp) * (f[i][0] - fp) / fp;
    Xi[1] += (f[i][1] - fp) * (f[i][1] - fp) / fp;
    Xi[2] += (f[i][2] - fp) * (f[i][2] - fp) / fp;
  }
  printf("R:%f G:%f B:%f\n", Xi[2], Xi[1], Xi[0]);
  fclose(fin);
}
int main() {
  char * s1, * s2, * s3, * s4;
  s1 = malloc(100);
  s2 = malloc(100);
  s3 = malloc(100);
  s4 = malloc(100);
  printf("Introduceti imaginea care urmeaza sa fie criptata:");
  scanf("%s", s1);
  printf("Introduceti numele imaginii criptate:");
  scanf("%s", s2);
  printf("Introduceti fisierul cu cheia secreta:");
  scanf("%s", s3);
  CriptareIMG(s1, s2, s3);
  printf("Introduceti numele imaginii decriptate:");
  scanf("%s", s4);
  DecriptareIMG(s2, s4, s3);
  printf("Canalele de culoare pentru imaginea initiala:\n");
  ChiPatrat(s1);
  printf("Canalele de culoare pentru imaginea criptata:\n");
  ChiPatrat(s2);
  free(s1);
  free(s2);
  free(s3);
  free(s4);
}
