#include "stb_dxt.h"

static unsigned char stb__Expand5[32];
static unsigned char stb__Expand6[64];
static unsigned char stb__OMatch5[256][2];
static unsigned char stb__OMatch6[256][2];
static unsigned char stb__QuantRBTab[256+16];
static unsigned char stb__QuantGTab[256+16];

int stb__Mul8Bit(int a, int b)
{
  int t = a*b + 128;
  return (t + (t >> 8)) >> 8;
}

void stb__From16Bit(unsigned char *out, unsigned short v)
{
   int rv = (v & 0xf800) >> 11;
   int gv = (v & 0x07e0) >>  5;
   int bv = (v & 0x001f) >>  0;

   out[0] = stb__Expand5[rv];
   out[1] = stb__Expand6[gv];
   out[2] = stb__Expand5[bv];
   out[3] = 0;
}

unsigned short stb__As16Bit(int r, int g, int b)
{
   return (stb__Mul8Bit(r,31) << 11) + (stb__Mul8Bit(g,63) << 5) + stb__Mul8Bit(b,31);
}

int stb__Lerp13(int a, int b)
{
#ifdef STB_DXT_USE_ROUNDING_BIAS
   // with rounding bias
   return a + stb__Mul8Bit(b-a, 0x55);
#else
   // without rounding bias
   // replace "/ 3" by "* 0xaaab) >> 17" if your compiler sucks or you really need every ounce of speed.
   return (2*a + b) / 3;
#endif
}

void stb__Lerp13RGB(unsigned char *out, unsigned char *p1, unsigned char *p2)
{
   out[0] = stb__Lerp13(p1[0], p2[0]);
   out[1] = stb__Lerp13(p1[1], p2[1]);
   out[2] = stb__Lerp13(p1[2], p2[2]);
}

void stb__PrepareOptTable(unsigned char *Table,const unsigned char *expand,int size)
{
   int i,mn,mx;
   for (i=0;i<256;i++) {
      int bestErr = 256;
      for (mn=0;mn<size;mn++) {
         for (mx=0;mx<size;mx++) {
            int mine = expand[mn];
            int maxe = expand[mx];
            int err = STBD_ABS(stb__Lerp13(maxe, mine) - i);
            
            // DX10 spec says that interpolation must be within 3% of "correct" result,
            // add this as error term. (normally we'd expect a random distribution of
            // +-1.5% error, but nowhere in the spec does it say that the error has to be
            // unbiased - better safe than sorry).
            err += STBD_ABS(maxe - mine) * 3 / 100;
            
            if(err < bestErr)
            { 
               Table[i*2+0] = mx;
               Table[i*2+1] = mn;
               bestErr = err;
            }
         }
      }
   }
}

void stb__EvalColors(unsigned char *color,unsigned short c0,unsigned short c1)
{
   stb__From16Bit(color+ 0, c0);
   stb__From16Bit(color+ 4, c1);
   stb__Lerp13RGB(color+ 8, color+0, color+4);
   stb__Lerp13RGB(color+12, color+4, color+0);
}

void stb__DitherBlock(unsigned char *dest, unsigned char *block)
{
  int err[8],*ep1 = err,*ep2 = err+4, *et;
  int ch,y;

  // process channels separately
  for (ch=0; ch<3; ++ch) {
      unsigned char *bp = block+ch, *dp = dest+ch;
      unsigned char *quant = (ch == 1) ? stb__QuantGTab+8 : stb__QuantRBTab+8;
      STBD_MEMSET(err, 0, sizeof(err));
      for(y=0; y<4; ++y) {
         dp[ 0] = quant[bp[ 0] + ((3*ep2[1] + 5*ep2[0]) >> 4)];
         ep1[0] = bp[ 0] - dp[ 0];
         dp[ 4] = quant[bp[ 4] + ((7*ep1[0] + 3*ep2[2] + 5*ep2[1] + ep2[0]) >> 4)];
         ep1[1] = bp[ 4] - dp[ 4];
         dp[ 8] = quant[bp[ 8] + ((7*ep1[1] + 3*ep2[3] + 5*ep2[2] + ep2[1]) >> 4)];
         ep1[2] = bp[ 8] - dp[ 8];
         dp[12] = quant[bp[12] + ((7*ep1[2] + 5*ep2[3] + ep2[2]) >> 4)];
         ep1[3] = bp[12] - dp[12];
         bp += 16;
         dp += 16;
         et = ep1, ep1 = ep2, ep2 = et; // swap
      }
   }
}

unsigned int stb__MatchColorsBlock(unsigned char *block, unsigned char *color,int dither)
{
   unsigned int mask = 0;
   int dirr = color[0*4+0] - color[1*4+0];
   int dirg = color[0*4+1] - color[1*4+1];
   int dirb = color[0*4+2] - color[1*4+2];
   int dots[16];
   int stops[4];
   int i;
   int c0Point, halfPoint, c3Point;

   for(i=0;i<16;i++)
      dots[i] = block[i*4+0]*dirr + block[i*4+1]*dirg + block[i*4+2]*dirb;

   for(i=0;i<4;i++)
      stops[i] = color[i*4+0]*dirr + color[i*4+1]*dirg + color[i*4+2]*dirb;

   // think of the colors as arranged on a line; project point onto that line, then choose
   // next color out of available ones. we compute the crossover points for "best color in top
   // half"/"best in bottom half" and then the same inside that subinterval.
   //
   // relying on this 1d approximation isn't always optimal in terms of euclidean distance,
   // but it's very close and a lot faster.
   // http://cbloomrants.blogspot.com/2008/12/12-08-08-dxtc-summary.html
   
   c0Point   = (stops[1] + stops[3]) >> 1;
   halfPoint = (stops[3] + stops[2]) >> 1;
   c3Point   = (stops[2] + stops[0]) >> 1;

   if(!dither) {
      // the version without dithering is straightforward
      for (i=15;i>=0;i--) {
         int dot = dots[i];
         mask <<= 2;

         if(dot < halfPoint)
           mask |= (dot < c0Point) ? 1 : 3;
         else
           mask |= (dot < c3Point) ? 2 : 0;
      }
  } else {
      // with floyd-steinberg dithering
      int err[8],*ep1 = err,*ep2 = err+4;
      int *dp = dots, y;

      c0Point   <<= 4;
      halfPoint <<= 4;
      c3Point   <<= 4;
      for(i=0;i<8;i++)
         err[i] = 0;

      for(y=0;y<4;y++)
      {
         int dot,lmask,step;

         dot = (dp[0] << 4) + (3*ep2[1] + 5*ep2[0]);
         if(dot < halfPoint)
           step = (dot < c0Point) ? 1 : 3;
         else
           step = (dot < c3Point) ? 2 : 0;
         ep1[0] = dp[0] - stops[step];
         lmask = step;

         dot = (dp[1] << 4) + (7*ep1[0] + 3*ep2[2] + 5*ep2[1] + ep2[0]);
         if(dot < halfPoint)
           step = (dot < c0Point) ? 1 : 3;
         else
           step = (dot < c3Point) ? 2 : 0;
         ep1[1] = dp[1] - stops[step];
         lmask |= step<<2;

         dot = (dp[2] << 4) + (7*ep1[1] + 3*ep2[3] + 5*ep2[2] + ep2[1]);
         if(dot < halfPoint)
           step = (dot < c0Point) ? 1 : 3;
         else
           step = (dot < c3Point) ? 2 : 0;
         ep1[2] = dp[2] - stops[step];
         lmask |= step<<4;

         dot = (dp[3] << 4) + (7*ep1[2] + 5*ep2[3] + ep2[2]);
         if(dot < halfPoint)
           step = (dot < c0Point) ? 1 : 3;
         else
           step = (dot < c3Point) ? 2 : 0;
         ep1[3] = dp[3] - stops[step];
         lmask |= step<<6;

         dp += 4;
         mask |= lmask << (y*8);
         { int *et = ep1; ep1 = ep2; ep2 = et; } // swap
      }
   }

   return mask;
}

void stb__OptimizeColorsBlock(unsigned char *block, unsigned short *pmax16, unsigned short *pmin16)
{
  int mind = 0x7fffffff,maxd = -0x7fffffff;
  unsigned char *minp = NULL, *maxp = NULL;
  double magn;
  int v_r,v_g,v_b;
  static const int nIterPower = 4;
  float covf[6],vfr,vfg,vfb;

  // determine color distribution
  int cov[6];
  int mu[3],min[3],max[3];
  int ch,i,iter;

  for(ch=0;ch<3;ch++)
  {
    const unsigned char *bp = ((const unsigned char *) block) + ch;
    int muv,minv,maxv;

    muv = minv = maxv = bp[0];
    for(i=4;i<64;i+=4)
    {
      muv += bp[i];
      if (bp[i] < minv) minv = bp[i];
      else if (bp[i] > maxv) maxv = bp[i];
    }

    mu[ch] = (muv + 8) >> 4;
    min[ch] = minv;
    max[ch] = maxv;
  }

  // determine covariance matrix
  for (i=0;i<6;i++)
     cov[i] = 0;

  for (i=0;i<16;i++)
  {
    int r = block[i*4+0] - mu[0];
    int g = block[i*4+1] - mu[1];
    int b = block[i*4+2] - mu[2];

    cov[0] += r*r;
    cov[1] += r*g;
    cov[2] += r*b;
    cov[3] += g*g;
    cov[4] += g*b;
    cov[5] += b*b;
  }

  // convert covariance matrix to float, find principal axis via power iter
  for(i=0;i<6;i++)
    covf[i] = cov[i] / 255.0f;

  vfr = (float) (max[0] - min[0]);
  vfg = (float) (max[1] - min[1]);
  vfb = (float) (max[2] - min[2]);

  for(iter=0;iter<nIterPower;iter++)
  {
    float r = vfr*covf[0] + vfg*covf[1] + vfb*covf[2];
    float g = vfr*covf[1] + vfg*covf[3] + vfb*covf[4];
    float b = vfr*covf[2] + vfg*covf[4] + vfb*covf[5];

    vfr = r;
    vfg = g;
    vfb = b;
  }

  magn = STBD_FABS(vfr);
  if (STBD_FABS(vfg) > magn) magn = STBD_FABS(vfg);
  if (STBD_FABS(vfb) > magn) magn = STBD_FABS(vfb);

   if(magn < 4.0f) { // too small, default to luminance
      v_r = 299; // JPEG YCbCr luma coefs, scaled by 1000.
      v_g = 587;
      v_b = 114;
   } else {
      magn = 512.0 / magn;
      v_r = (int) (vfr * magn);
      v_g = (int) (vfg * magn);
      v_b = (int) (vfb * magn);
   }

   // Pick colors at extreme points
   for(i=0;i<16;i++)
   {
      int dot = block[i*4+0]*v_r + block[i*4+1]*v_g + block[i*4+2]*v_b;

      if (dot < mind) {
         mind = dot;
         minp = block+i*4;
      }

      if (dot > maxd) {
         maxd = dot;
         maxp = block+i*4;
      }
   }

   if (pmax16)
    *pmax16 = stb__As16Bit(maxp[0],maxp[1],maxp[2]);
   if (pmin16)
    *pmin16 = stb__As16Bit(minp[0],minp[1],minp[2]);
}

int stb__sclamp(float y, int p0, int p1)
{
   int x = (int) y;
   if (x < p0) return p0;
   if (x > p1) return p1;
   return x;
}

int stb__RefineBlock(unsigned char *block, unsigned short *pmax16, unsigned short *pmin16, unsigned int mask)
{
   static const int w1Tab[4] = { 3,0,2,1 };
   static const int prods[4] = { 0x090000,0x000900,0x040102,0x010402 };
   // ^some magic to save a lot of multiplies in the accumulating loop...
   // (precomputed products of weights for least squares system, accumulated inside one 32-bit register)

   float frb,fg;
   unsigned short oldMin, oldMax, min16, max16;
   int i, akku = 0, xx,xy,yy;
   int At1_r,At1_g,At1_b;
   int At2_r,At2_g,At2_b;
   unsigned int cm = mask;

   oldMin = *pmin16;
   oldMax = *pmax16;

   if((mask ^ (mask<<2)) < 4) // all pixels have the same index?
   {
      // yes, linear system would be singular; solve using optimal
      // single-color match on average color
      int r = 8, g = 8, b = 8;
      for (i=0;i<16;++i) {
         r += block[i*4+0];
         g += block[i*4+1];
         b += block[i*4+2];
      }

      r >>= 4; g >>= 4; b >>= 4;

      max16 = (stb__OMatch5[r][0]<<11) | (stb__OMatch6[g][0]<<5) | stb__OMatch5[b][0];
      min16 = (stb__OMatch5[r][1]<<11) | (stb__OMatch6[g][1]<<5) | stb__OMatch5[b][1];
   } else {
      At1_r = At1_g = At1_b = 0;
      At2_r = At2_g = At2_b = 0;
      for (i=0;i<16;++i,cm>>=2) {
         int step = cm&3;
         int w1 = w1Tab[step];
         int r = block[i*4+0];
         int g = block[i*4+1];
         int b = block[i*4+2];

         akku    += prods[step];
         At1_r   += w1*r;
         At1_g   += w1*g;
         At1_b   += w1*b;
         At2_r   += r;
         At2_g   += g;
         At2_b   += b;
      }

      At2_r = 3*At2_r - At1_r;
      At2_g = 3*At2_g - At1_g;
      At2_b = 3*At2_b - At1_b;

      // extract solutions and decide solvability
      xx = akku >> 16;
      yy = (akku >> 8) & 0xff;
      xy = (akku >> 0) & 0xff;

      frb = 3.0f * 31.0f / 255.0f / (xx*yy - xy*xy);
      fg = frb * 63.0f / 31.0f;

      // solve.
      max16 =   stb__sclamp((At1_r*yy - At2_r*xy)*frb+0.5f,0,31) << 11;
      max16 |=  stb__sclamp((At1_g*yy - At2_g*xy)*fg +0.5f,0,63) << 5;
      max16 |=  stb__sclamp((At1_b*yy - At2_b*xy)*frb+0.5f,0,31) << 0;

      min16 =   stb__sclamp((At2_r*xx - At1_r*xy)*frb+0.5f,0,31) << 11;
      min16 |=  stb__sclamp((At2_g*xx - At1_g*xy)*fg +0.5f,0,63) << 5;
      min16 |=  stb__sclamp((At2_b*xx - At1_b*xy)*frb+0.5f,0,31) << 0;
   }

   *pmin16 = min16;
   *pmax16 = max16;
   return oldMin != min16 || oldMax != max16;
}

void stb__CompressColorBlock(unsigned char *dest, unsigned char *block, int mode)
{
   unsigned int mask;
   int i;
   int dither;
   int refinecount;
   unsigned short max16 = 0, min16 = 0;
   unsigned char dblock[16*4],color[4*4];
   
   dither = mode & STB_DXT_DITHER;
   refinecount = (mode & STB_DXT_HIGHQUAL) ? 2 : 1;

   // check if block is constant
   for (i=1;i<16;i++)
      if (((unsigned int *) block)[i] != ((unsigned int *) block)[0])
         break;

   if(i == 16) { // constant color
      int r = block[0], g = block[1], b = block[2];
      mask  = 0xaaaaaaaa;
      max16 = (stb__OMatch5[r][0]<<11) | (stb__OMatch6[g][0]<<5) | stb__OMatch5[b][0];
      min16 = (stb__OMatch5[r][1]<<11) | (stb__OMatch6[g][1]<<5) | stb__OMatch5[b][1];
   } else {
      // first step: compute dithered version for PCA if desired
      if(dither)
         stb__DitherBlock(dblock,block);

      // second step: pca+map along principal axis
      stb__OptimizeColorsBlock(dither ? dblock : block,&max16,&min16);
      if (max16 != min16) {
         stb__EvalColors(color,max16,min16);
         mask = stb__MatchColorsBlock(block,color,dither);
      } else
         mask = 0;

      // third step: refine (multiple times if requested)
      for (i=0;i<refinecount;i++) {
         unsigned int lastmask = mask;
         
         if (stb__RefineBlock(dither ? dblock : block,&max16,&min16,mask)) {
            if (max16 != min16) {
               stb__EvalColors(color,max16,min16);
               mask = stb__MatchColorsBlock(block,color,dither);
            } else {
               mask = 0;
               break;
            }
         }
         
         if(mask == lastmask)
            break;
      }
  }

  // write the color block
  if(max16 < min16)
  {
     unsigned short t = min16;
     min16 = max16;
     max16 = t;
     mask ^= 0x55555555;
  }

  dest[0] = (unsigned char) (max16);
  dest[1] = (unsigned char) (max16 >> 8);
  dest[2] = (unsigned char) (min16);
  dest[3] = (unsigned char) (min16 >> 8);
  dest[4] = (unsigned char) (mask);
  dest[5] = (unsigned char) (mask >> 8);
  dest[6] = (unsigned char) (mask >> 16);
  dest[7] = (unsigned char) (mask >> 24);
}

void stb__CompressAlphaBlock(unsigned char *dest,unsigned char *src, int stride)
{
   int i,dist,bias,dist4,dist2,bits,mask;

   // find min/max color
   int mn,mx;
   mn = mx = src[0];

   for (i=1;i<16;i++)
   {
      if (src[i*stride] < mn) mn = src[i*stride];
      else if (src[i*stride] > mx) mx = src[i*stride];
   }

   // encode them
   ((unsigned char *)dest)[0] = mx;
   ((unsigned char *)dest)[1] = mn;
   dest += 2;

   // determine bias and emit color indices
   // given the choice of mx/mn, these indices are optimal:
   // http://fgiesen.wordpress.com/2009/12/15/dxt5-alpha-block-index-determination/
   dist = mx-mn;
   dist4 = dist*4;
   dist2 = dist*2;
   bias = (dist < 8) ? (dist - 1) : (dist/2 + 2);
   bias -= mn * 7;
   bits = 0,mask=0;
   
   for (i=0;i<16;i++) {
      int a = src[i*stride]*7 + bias;
      int ind,t;

      // select index. this is a "linear scale" lerp factor between 0 (val=min) and 7 (val=max).
      t = (a >= dist4) ? -1 : 0; ind =  t & 4; a -= dist4 & t;
      t = (a >= dist2) ? -1 : 0; ind += t & 2; a -= dist2 & t;
      ind += (a >= dist);
      
      // turn linear scale into DXT index (0/1 are extremal pts)
      ind = -ind & 7;
      ind ^= (2 > ind);

      // write index
      mask |= ind << bits;
      if((bits += 3) >= 8) {
         *dest++ = mask;
         mask >>= 8;
         bits -= 8;
      }
   }
}

void stb__InitDXT()
{
   int i;
   for(i=0;i<32;i++)
      stb__Expand5[i] = (i<<3)|(i>>2);

   for(i=0;i<64;i++)
      stb__Expand6[i] = (i<<2)|(i>>4);

   for(i=0;i<256+16;i++)
   {
      int v = i-8 < 0 ? 0 : i-8 > 255 ? 255 : i-8;
      stb__QuantRBTab[i] = stb__Expand5[stb__Mul8Bit(v,31)];
      stb__QuantGTab[i] = stb__Expand6[stb__Mul8Bit(v,63)];
   }

   stb__PrepareOptTable(&stb__OMatch5[0][0],stb__Expand5,32);
   stb__PrepareOptTable(&stb__OMatch6[0][0],stb__Expand6,64);
}

void stb_compress_dxt_block(unsigned char *dest, const unsigned char *src, int alpha, int mode)
{
   unsigned char data[16][4];
   static int init=1;
   if (init) {
      stb__InitDXT();
      init=0;
   }

   if (alpha) {
      int i;
      stb__CompressAlphaBlock(dest,(unsigned char*) src+3, 4);
      dest += 8;
      // make a new copy of the data in which alpha is opaque,
      // because code uses a fast test for color constancy
      memcpy(data, src, 4*16);
      for (i=0; i < 16; ++i)
         data[i][3] = 255;
      src = &data[0][0];
   }

   stb__CompressColorBlock(dest,(unsigned char*) src,mode);
}

void stb_compress_bc4_block(unsigned char *dest, const unsigned char *src)
{
    static int init = 1;
    if (init) {
        stb__InitDXT();
        init = 0;
    }

   stb__CompressAlphaBlock(dest,(unsigned char*) src, 1);
}

void stb_compress_bc5_block(unsigned char *dest, const unsigned char *src)
{
   stb__CompressAlphaBlock(dest,(unsigned char*) src,2);
   stb__CompressAlphaBlock(dest + 8,(unsigned char*) src+1,2);
}

int imin(int x, int y) { 
  return (x < y) ? x : y; 
}

void extract_block(const unsigned char *src, int x, int y, int w, int h, unsigned char *block) {

  int i, j;
  if ((w-x >=4) && (h-y >=4)){

    src += x*4;
    src += y*w*4;
    for (i=0; i < 4; ++i)
    {
      *(unsigned int*)block = *(unsigned int*) src; block += 4; src += 4;
      *(unsigned int*)block = *(unsigned int*) src; block += 4; src += 4;
      *(unsigned int*)block = *(unsigned int*) src; block += 4; src += 4;
      *(unsigned int*)block = *(unsigned int*) src; block += 4;
      src += (w*4) - 12;
    }
    return;
  }

  int bw = imin(w - x, 4);
  int bh = imin(h - y, 4);
  int bx, by;

  const int rem[] =
  {
    0, 0, 0, 0,
    0, 1, 0, 1,
    0, 1, 2, 0,
    0, 1, 2, 3
  };

  for(i = 0; i < 4; ++i){
    by = rem[(bh - 1) * 4 + i] + y;
    for(j = 0; j < 4; ++j){
      bx = rem[(bw - 1) * 4 + j] + x;
      block[(i * 4 * 4) + (j * 4) + 0] = src[(by * (w * 4)) + (bx * 4) + 0];
      block[(i * 4 * 4) + (j * 4) + 1] = src[(by * (w * 4)) + (bx * 4) + 1];
      block[(i * 4 * 4) + (j * 4) + 2] = src[(by * (w * 4)) + (bx * 4) + 2];
      block[(i * 4 * 4) + (j * 4) + 3] = src[(by * (w * 4)) + (bx * 4) + 3];
    }
  }
}

uint64_t get_compress_pixels_dst_size_bytes(uint64_t w, uint64_t h, bool is_rgba) {

  if (is_rgba) {
    return w * h;
  } else {
    return w * h / 2;
  }
  return 0;
}

bool compress_pixels(unsigned char* dst, const unsigned char* src, uint64_t w, uint64_t h, bool is_rgba) {

  // Cannot compress really small images or images that
  // do not have lengths that are a multiple of 4.
  if (w < 4 || w%4 != 0 || h < 4 || h%4 != 0) return false;

  unsigned char block[64];
  int x, y;
  for(y = 0; y < h; y += 4){
    for(x = 0; x < w; x += 4){
      extract_block(src, x, y, w, h, block);
      stb_compress_dxt_block(dst, block, is_rgba, STB_DXT_HIGHQUAL);
      dst += is_rgba ? 16 : 8; //advance pointer to next block
    }
  }
  return true;
}

bool compress_pixelsBC4(unsigned char* dst, const unsigned char* src, uint64_t w, uint64_t h, bool is_rgba) {

    // Cannot compress really small images or images that
    // do not have lengths that are a multiple of 4.
    if (w < 4 || w % 4 != 0 || h < 4 || h % 4 != 0) return false;

    unsigned char block[64];
    int x, y;
    for (y = 0; y < h; y += 4) {
        for (x = 0; x < w; x += 4) {
            extract_block(src, x, y, w, h, block);
            stb_compress_bc4_block(dst, block);
            dst += 8; //advance pointer to next block
        }
    }
    return true;
}