#include <iostream>
#include <math.h>
#include <sndfile.h>
#include <samplerate.h>
using namespace std;

int main(int argc, char *argv[])
{
  // arg check
  if(argc != 4)
    {
      cerr << "wrong usage" << endl;
      cout << "usage: ./transposer audiofilein.wav audiofileout.wav transpositioninsemitones" << endl;
      return 1;
    }
  char* fileinpath  = argv[1];
  char* fileoutpath = argv[2];
  float transposition;  
  if(sscanf(argv[3], "%f", &transposition) != 1)
    {
      cerr << "invalid transposition" << endl;
      return 1;
    }
  
  // read complete audio file into memory
  SF_INFO sfinfoIn;
  SNDFILE* sndfileIn = sf_open(fileinpath, SFM_READ, &sfinfoIn);
  if(sndfileIn==NULL)
    {
      cerr << "cannot read input sound file" << endl;
      return 1;
    }
  int datainLen      = sfinfoIn.frames;
  int channels       = sfinfoIn.channels; 
  float* dataIn = new float[datainLen * channels];
  sf_readf_float(sndfileIn, dataIn, datainLen);

  // allocate output buffer
  int dataoutLen     = (int)(datainLen * pow(2., -1./12 * transposition)) + 1;
  cout << "data out len: " << dataoutLen << endl;
  float* dataOut = new float[dataoutLen * channels];

  // convert
  SRC_DATA data;
  data.data_in = dataIn;
  data.data_out = dataOut;
  data.input_frames = datainLen;
  data.output_frames = dataoutLen;
  data.src_ratio = pow(2., -1./12 * transposition);
  int err = src_simple (&data, SRC_SINC_MEDIUM_QUALITY, channels) ;
  cout << "err = " << err << endl;

  // write output file
  SF_INFO sfinfoOut;
  sfinfoOut.frames = dataoutLen;
  sfinfoOut.channels = channels;
  sfinfoOut.format = sfinfoIn.format;
  sfinfoOut.samplerate = sfinfoIn.samplerate;
  SNDFILE* sndfileOut = sf_open(fileoutpath, SFM_WRITE, &sfinfoOut);
  if(sndfileOut == NULL)
      cerr << "cannot write output sound file" << endl;
  else 
    {
      sf_writef_float(sndfileOut, dataOut, dataoutLen);
      sf_close(sndfileOut);
    }

  // release memory, file handles
  if(dataIn != NULL)
    delete[] dataIn;
  if(dataOut != NULL)
    delete[] dataOut;
  sf_close(sndfileIn);
  return 0;
}
