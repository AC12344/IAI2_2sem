/**
 * @author Poramate 02.12.2013
 */

#include "ann_xnor.h"
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// ----------------------------------------------------------------------
// ------------ Initial constructor -------------------------------------
// ----------------------------------------------------------------------

ANN_XNOR::ANN_XNOR()
{

  srand (time(NULL));
 n = 0;
  //Random weights of input to hidden units

  for (int i=0;i<numberHidden;i++) {
    for (int j=0;j<numberInput;j++) {
      // For some reason, the Microsoft rand() function
      // generates a random integer. So, I divide by the
      // number by MAXINT/2, to get a num between 0 and 2,
      // the subtract one to get a num between -1 and 1.

      WeightH_I[i][j] = (double)(rand()%1000)/1000.0; // weigths for input, this ne was 5 in the current network, 2 from input to output, 2 from input to h and on from h to outut
      WeightH_B[i] = (double)(rand()%1000)/1000.0; // weight for bias term - there is only 2, one from input and one from hidden

      a_Hidden[i] = 0.0;
      o_Hidden[i] = 0.0;
      BiasH[i] = 1.0;

      DeltaWeightH_I[i][j]= 0.0;

    }
  }


  //Random weights of hidden to output units

  for (int o=0;o<numberOutput;o++) {
    for (int h=0;h<numberHidden;h++) {
      // For some reason, the Microsoft rand() function
      // generates a random integer. So, I divide by the
      // number by MAXINT/2, to get a num between 0 and 2,
      // the subtract one to get a num between -1 and 1.

      WeightO_B[o] = (double)(rand()%1000)/1000.0;
      WeightO_H [o][h] = (double)(rand()%1000)/1000.0;
      a_Output[o]= 0.0;
      o_Output[o]= 0.0;
      BiasO[o] = 1.0;

      DeltaWeightH_B[h] = 1.0;
      DeltaWeightO_B[o] = 0.5;
      DeltaWeightO_H [o][h] = 0.2;

    }
  }


  //Random weights of input to output units

  for (int oi=0;oi<numberOutput;oi++) {
    for (int ii=0;ii<numberInput;ii++) {
      // For some reason, the Microsoft rand() function
      // generates a random integer. So, I divide by the
      // number by MAXINT/2, to get a num between 0 and 2,
      // the subtract one to get a num between -1 and 1.

      WeightO_I[oi][ii] = (double)(rand()%1000)/1000.0;
      DeltaWeightO_I[ii] = 0.0;
    }
  }


  deltaOutput = 0.0;
  deltaHidden[0] = 0.0;

  saveFile1.open("error.txt",ios::out);

}

// ----------------------------------------------------------------------
// ------------ destructor ----------------------------------------------
// ----------------------------------------------------------------------

ANN_XNOR::~ANN_XNOR()
{

}


// ----------------------------------------------------------------------
// --- Multilayer Feedforward Neural Networks  --------------------------
// ----------------------------------------------------------------------
double ANN_XNOR::FeedforwardNetwork (double i0, double i1, double d)

/* Nonlinear seperate ///XOR = AND + OR
1. in_j = sumk(w_k,j * a_k) // Forward pass starts. Compute weighed
2. a_j = sigmoid(in_j) // Compute outputs at all hidde
3. in_i = sumj(w_j,i * a_j) // Compute weighed inputs to all output u
4. a_i = sigmoid(in_i) // Compute outputs at all output units
5. del_i = a_i * (1 - a_i) * (y_i - a_i) // Backward pass starts. Compute "modified error" a
6. del_j = a_j * (1 - a_j) * sumi(w_j,i * del_i) // Compute "modified error" at all hidden units
7. w_j,i = w_j,i + (alpha * a_j * del_i) // update weights between hidden and output units
8. w_k,j = w_k,j + (alpha * a_k * del_j) // update weights between input and hidden units

 */

{
  

  //STEP 1: Present input pattern [2 inputs for XNOR ]//////////////

  Input[0] = i0; // Set input
  Input[1] = i1; // Set input

  //STEP 2: Construct net [1 hidden, 1 output] ////////////////////

  // Calculate the net values for the hidden layer neurons.
  //Activation
  a_Hidden[0] = BiasH[0]*WeightH_B[0]+Input[0]*WeightH_I[0][0] + Input[1]*WeightH_I[0][1];

  //Activity
  o_Hidden[0]  = sigmoid(a_Hidden[0]);


  // Now, calculate the net for the final output layer.
  //Activation
  a_Output[0] = BiasO[0]*WeightO_B[0]+o_Hidden[0]*WeightO_H[0][0] + Input[0]*WeightO_I[0][0] + Input[1]*WeightO_I[0][1];

  //Activity
  o_Output[0] = tanh(a_Output[0]);


  //STEP 3: Cal error (delta at output and delta at hidden)///////

  // We have to calculate the deltas for the two layers.
  // Remember, we have to calculate the errors backwards
  // from the output layer to the hidden layer (thus the
  // name 'BACK-propagation').


  //CALCULATE the DELTAS//
  //[1] find the delta of the output layer = error between Target T and Output = d-o_Output[0]:

  //Sigmoid (logistic function)
  //error = d-o_Output[0];
  //deltaOutput = o_Output[0]*(1.-o_Output[0])*(error); //From output layer

  // tanh - it depends on the the derivative of the f(a) - in this case tanh
  error = d-o_Output[0];
  deltaOutput = (1. - o_Output[0]*o_Output[0])*error; // from output layer 

  //[2] find the delta of the hidden layer
  deltaHidden[0] = o_Hidden[0]*(1.-o_Hidden[0])*(WeightO_H[0][0]*deltaOutput);

  //STEP 4: Update weights for Online learning//////////////

  ///////////Output////////////////////////////////////////////

  if(n == 0) 
  {
     DeltaWeightO_H[0][0] = BP_LEARNING*deltaOutput*o_Hidden[0];
     WeightO_H[0][0] = WeightO_H[0][0]+DeltaWeightO_H[0][0];

    DeltaWeightO_B[0] = BP_LEARNING*deltaOutput*BiasO[0];
    WeightO_B[0] = WeightO_B[0]+DeltaWeightO_B[0];

    DeltaWeightO_I[0] = BP_LEARNING*deltaOutput*Input[0];
    WeightO_I[0][0] = WeightO_I[0][0]+DeltaWeightO_I[0];

    DeltaWeightO_I[1] = BP_LEARNING*deltaOutput*Input[1];
    WeightO_I[0][1] = WeightO_I[0][1]+DeltaWeightO_I[1];
	  /////////Hidden0//////////////////////////////////////////////
    DeltaWeightH_B[0] = BP_LEARNING*deltaHidden[0]*BiasH[0];
    WeightH_B[0] = WeightH_B[0]+DeltaWeightH_B[0];

    DeltaWeightH_I[0][0] = BP_LEARNING*deltaHidden[0]*Input[0];
    WeightH_I[0][0] = WeightH_I[0][0]+DeltaWeightH_I[0][0];

    DeltaWeightH_I[0][1] = BP_LEARNING*deltaHidden[0]*Input[1];
    WeightH_I[0][1] = WeightH_I[0][1] + DeltaWeightH_I[0][1];

    n++;
  }
  else
  {
	  DeltaWeightO_H[0][0] = BP_LEARNING*deltaOutput*o_Hidden[0];
  	WeightO_H[0][0] = WeightO_H[0][0]+DeltaWeightO_H[0][0] + momentum_rate*DeltaWeightO_H[0][0];

  	DeltaWeightO_B[0] = BP_LEARNING*deltaOutput*BiasO[0];
  	WeightO_B[0] = WeightO_B[0]+DeltaWeightO_B[0] +  momentum_rate*DeltaWeightO_B[0];

  	DeltaWeightO_I[0] = BP_LEARNING*deltaOutput*Input[0];
  	WeightO_I[0][0] = WeightO_I[0][0]+DeltaWeightO_I[0] + momentum_rate*DeltaWeightO_I[0];

  	DeltaWeightO_I[1] = BP_LEARNING*deltaOutput*Input[1];
  	WeightO_I[0][1] = WeightO_I[0][1]+DeltaWeightO_I[1] + momentum_rate*DeltaWeightO_I[1];

  /////////Hidden0//////////////////////////////////////////////
  	DeltaWeightH_B[0] = BP_LEARNING*deltaHidden[0]*BiasH[0];
  	WeightH_B[0] = WeightH_B[0]+DeltaWeightH_B[0] + momentum_rate*DeltaWeightH_B[0];

  	DeltaWeightH_I[0][0] = BP_LEARNING*deltaHidden[0]*Input[0];
  	WeightH_I[0][0] = WeightH_I[0][0]+DeltaWeightH_I[0][0] + momentum_rate*DeltaWeightH_I[0][0];

  	DeltaWeightH_I[0][1] = BP_LEARNING*deltaHidden[0]*Input[1];
  	WeightH_I[0][1] = WeightH_I[0][1] + DeltaWeightH_I[0][1] + momentum_rate*DeltaWeightH_I[0][1];

	  n++;
  }

/*  DeltaWeightO_H[0][0] = BP_LEARNING*deltaOutput*o_Hidden[0];
  WeightO_H[0][0] = WeightO_H[0][0]+DeltaWeightO_H[0][0] + momentum_rate*DeltaWeightO_H[0][0];
  
  DeltaWeightO_B[0] = BP_LEARNING*deltaOutput*BiasO[0];
  WeightO_B[0] = WeightO_B[0]+DeltaWeightO_B[0]; // + momentum_rate*WeightO_B[0];
  
  DeltaWeightO_I[0] = BP_LEARNING*deltaOutput*Input[0];
  WeightO_I[0][0] = WeightO_I[0][0]+DeltaWeightO_I[0]; // + momentum_rate*WeightO_I[0][0];

  DeltaWeightO_I[1] = BP_LEARNING*deltaOutput*Input[1];
  WeightO_I[0][1] = WeightO_I[0][1]+DeltaWeightO_I[1]; // + momentum_rate*WeightO_I[0][1];

  /////////Hidden0//////////////////////////////////////////////
  DeltaWeightH_B[0] = BP_LEARNING*deltaHidden[0]*BiasH[0];
  WeightH_B[0] = WeightH_B[0]+DeltaWeightH_B[0]; // + momentum_rate*WeightH_B[0];

  DeltaWeightH_I[0][0] = BP_LEARNING*deltaHidden[0]*Input[0];
  WeightH_I[0][0] = WeightH_I[0][0]+DeltaWeightH_I[0][0]; // + momentum_rate*WeightH_I[0][0];

  DeltaWeightH_I[0][1] = BP_LEARNING*deltaHidden[0]*Input[1];
  WeightH_I[0][1] = WeightH_I[0][1] + DeltaWeightH_I[0][1]; // + momentum_rate*WeightH_I[0][1];

*/
  printf( "error:%f W: %f %f %f %f %f %f %f \n",error*error,WeightH_B[0],WeightH_I[0][0],WeightH_I[0][1],
      WeightO_I[0][0],WeightO_I[0][1],WeightO_B[0],WeightO_H[0][0]);


  //printf( "I1:%f I2:%f Out:%f \n",Input[0], Input[1], o_Output[0]);

  saveFile1 <<error*error
      <<"  "<<WeightH_B[0]
                        <<"  "<<WeightH_I[0][0]
                                             <<"  "<<WeightH_I[0][1]
                                                                  <<"  "<<WeightO_I[0][0]
                                                                                       <<" "<<WeightO_I[0][1]
                                                                                                           <<" "<<WeightO_B[0]
                                                                                                                            <<" "<<WeightO_H[0][0]
                                                                                                                                                <<"   \n" << flush; //SAVE DATA


  return o_Output[0];


}

// ----------------------------------------------------------------------
// ------------ Neural controller ---------------------------------------
// ----------------------------------------------------------------------
double ANN_XNOR::Run(double i0, double i1)
{
	
/* weight given by our prof
  WeightH_B[0] = 2.64;
  WeightH_I[0][0] = -7.092;
  WeightH_I[0][1] = -7.09;
  WeightO_I[0][0] = 5.14;
  WeightO_I[0][1] = 5.135;
  WeightO_B[0] = -7.834;
  WeightO_H[0][0]= 11.575;
*/	
	// my weights - without momentum - works 
  WeightH_B[0] = 1.959647; 
  WeightH_I[0][0] = -5.669322; 
  WeightH_I[0][1] = -5.673477; 
  WeightO_I[0][0] = 3.420976; 
  WeightO_I[0][1] = 3.421749; 
  WeightO_B[0] = -3.610359; 
  WeightO_H[0][0]= 8.041114; 

  // my weigths with momentum 
 WeightH_B[0] = 2.637044; 
 WeightH_I[0][0] = -4.883417; 
 WeightH_I[0][1] = 5.548925; 
 WeightO_I[0][0] = 2.683116; 
 WeightO_I[0][1] = -2.935159; 
 WeightO_B[0] = -3.277309; 
 WeightO_H[0][0]= 6.214680; 


  ///////////////Start ///////////////////////////////////////////

  double p = 0.0;
  Input[0] =  i0; // Set input
  Input[1] =  i1; // Set input


  //////////Construct net//////////////////////////////////////////

  // Calculate the net values for the hidden layer neurons.
  a_Hidden[0] = BiasH[0] * WeightH_B[0] + Input[0] * WeightH_I[0][0] + Input[1] * WeightH_I[0][1];

  o_Hidden[0]  = sigmoid(a_Hidden[0]);


  // Now, calculate the net for the final output layer.
  a_Output[0] = BiasO[0] * WeightO_B[0] + o_Hidden[0] * WeightO_H[0][0] + Input[0] * WeightO_I[0][0] + Input[1] * WeightO_I[0][1];

  o_Output[0]= sigmoid(a_Output[0]);//sigmoid(a_Output[0]);


  printf( "I1:%f I2:%f Out:%f \n",Input[0], Input[1], o_Output[0]);

  return o_Output[0];

}



double ANN_XNOR::sigmoid(double num)
{
  return 1./(1.+exp(-num));
}

double ANN_XNOR::tanh(double num)
{	return sinh(num)/cosh(num);}

double ANN_XNOR::step(double num)
{
	if(num >= 0.5)
		return 1;
	else
		return 0;
}


