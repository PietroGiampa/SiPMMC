#include <sstream>

TString GetPDEAsString(double pde)
{
  TString npde = "";

  //If pde is 1, format the output as 1p0 rather than 1
  if (pde==1.0) {
    npde="1p0";
    return npde;
  }

  //convert pde to a string
  string out_string;
  stringstream ss;
  ss << pde;
  out_string = ss.str();

  //get a new string without a decimal point
  int digits = out_string.length();
  for (int i=0;i<digits;i++){
    //converting out_string[i] to const char *
    string sym(1, out_string[i]);
    const char * digit;
    digit = sym.c_str();

    if (std::strcmp(digit, ".")==0) npde.Append("p");
    else if (std::strcmp(digit, "0")==0) npde.Append("0");
    else if (std::strcmp(digit, "1")==0) npde.Append("1");
    else if (std::strcmp(digit, "2")==0) npde.Append("2");
    else if (std::strcmp(digit, "3")==0) npde.Append("3");
    else if (std::strcmp(digit, "4")==0) npde.Append("4");
    else if (std::strcmp(digit, "5")==0) npde.Append("5");
    else if (std::strcmp(digit, "6")==0) npde.Append("6");
    else if (std::strcmp(digit, "7")==0) npde.Append("7");
    else if (std::strcmp(digit, "8")==0) npde.Append("8");
    else if (std::strcmp(digit, "9")==0) npde.Append("9");
  }

  return npde;
}
