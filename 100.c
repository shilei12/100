#include<fstream>                                                        //the definations of ifstream and ofstream are included in the header file <fstream>
#include<iostream>
using namespace std;    

int main()              //define the main function                                                       
{  
	int i,n,m=0;                                                  //declare a serise variables of the type of int and double
	double a,b,c,a1,a2,a3,b1,b2,b3,c1,c2,c3,d1,d2,d3,y1;
	double temp[100];                                               //declare a array of the type of int to store the date that are read from the file named "f1.txt"
	ifstream infile("f1.txt", ios::in);                          //declare a input object infile to open the fle "f1.txt" to read the date and store them into the system
	ofstream outfile("f2.txt",ios::out);                         //declare a output object outfile to open the file "f2.txt" to write the date from the system to the file "f2.txt"
	if(!infile|!outfile)                                         //if there are not "f1.txt" and "f2.txt", the open operation fail
	{
		cerr<<"Open error!"<<endl;
		exit(1);            
	}
	do                                                                                  //provide two chooses to the user
	{
		cout<<"Which curve do you want to fit?"<<endl;
	    cout<<"1. straight line. 2. a second order polynomial equation.(Please input an integer from 1 and 2 to represent your choice)"<<endl;
	    cin>>i;                                                                        //input the number to represent the choose
		system("cls");
	}while(i!=1&&i!=2);                                                               //if the input is not appropraite, back to the interface and input number again
	a1=0,a2=0,a3=0,b1=0,b2=0,b3=0,c1=0,c2=0,c3=0,d1=0,d2=0,d3=0;
	if(i==1)                                                          
	 {
		 outfile<<"straight line"<<endl;                             //to choose to fit line
	     for(n=0;!infile.eof();n++)
		{
			infile>>temp[n];                                        //using for loop to read and store all numbers from the "f1.txt" file to the array temp[100]
			m++;                                                    //using the variable to represent the the number of date in "f1.txt"
		 }
		 infile.close(); //close "f1.txt"
		 
		 for(n=0;n<m;n=n+2)
		 { a1=a1+temp[n]*temp[n];             //to calculate the dependent variable according to the given formula
		   a2=a2+temp[n];
		   b1=a2;
		   b2=m/2;
		   d1=d1-temp[n]*temp[n+1];
		   d2=d2-temp[n+1];
		 }
     
		a=(b1*d2-b2*d1)/(a1*b2-a2*b1);      //gain the value of a and b
        b=(a1*d2-a2*d1)/(a1*b2-a2*b1);
		outfile<<"Cofficients of the derived equation are: "<<a<<"	"<<b<<endl;   //output the value of the best cofficient a and b
		for(n=0;n<m; n=n+2)
		{
			y1=a*temp[n]+b;                                                     //calculate the value of y according the value of correspoding x and the cofficients
			outfile<<temp[n]<<"	"<<temp[n+1]<<"	"<<y1<<endl;              
		}
	}

	  if(i==2)                                                                
	{    
		outfile<<"a second order polynomial equation"<<endl;                    //choose to fit polynomial equation
	    for(n=0;!infile.eof();n++)   
		{
			infile>>temp[n];
			m++;
		 }
		 infile.close();
		 
		 for(n=0;n<m;n=n+2)                                                     //gain the value of the dependent variables according the formular given by the question
		 {
			 a1=a1+temp[n]*temp[n]*temp[n]*temp[n];
			 a2=a2+temp[n]*temp[n]+temp[n];
			 a3=a3+temp[n]*temp[n];
			 b1=a2;
			 b2=a3;
			 b3=b3+temp[n];
			 c1=b2;
			 c2=b3;
			 c3=m/2;
			 d1=d1-temp[n]*temp[n]*temp[n+1];
			 d2=d2-temp[n]*temp[n+1];
			 d3=d3-temp[n+1];
		 }                                          //gain the value of the best coefficients a, b, c
         a=-((b1*(c2*d3-c3*d2)-c1*(b2*d3-b3*d2)+d1*(b2*c3-b3*c2))/(a1*(b2*c3-b3*c2)-b1*(a2*c3-a3*c2)+c1*(a2*b3-a3*b2)));
		 b=(a1*(c2*d3-c3*d2)-c1*(a2*d3-a3*d2)+d1*(a2*c3-a3*c2))/(a1*(b2*c3-b3*c2)-b1*(a2*c3-a3*c2)+c1*(a2*b3-a3*b2));
		 c=-((a1*(b2*d3-b3*d2)-b1*(a2*d3-a3*d2)+d1*(a2*b3-a3*b2))/(a1*(b2*c3-b3*c2)-b1*(a2*c3-a3*c2)+c1*(a2*b3-a3*b2)));

		 outfile<<"Cofficients of the derived equation are: "<<a<<"	"<<b<<"	"<<c<<endl;    //output the value of the best coefficients a , b and c
		for(n=0;n<m; n=n+2)      
		{
			y1=a*temp[n]*temp[n]+b*temp[n]+c;                         //calculate the value of y according the the corresponding x and the best coefficients
			outfile<<temp[n]<<"	"<<temp[n+1]<<"	"<<y1<<endl;         
		}
	  }                                                              //output the value of x, y and the cy calculated by the formula
	  
	    //reminder the user that the fitting program has been done
	  cout<<"The program has finished the fitting process."<<endl;
	      
	  
	  return 0;   

}