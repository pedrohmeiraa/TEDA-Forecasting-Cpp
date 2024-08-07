#include <iostream>
using namespace std;

#include "teda.h"
#include "rls.h"

const int nA = 300;

float A[nA] = {6., 13., 18., 23., 28., 33., 37., 42., 44., 43., 41., 39., 32.,
            23., 20., 21., 25., 29., 35., 40., 43., 46., 49., 51., 53., 55.,
            57., 59., 61., 62., 61., 62., 61., 58., 56., 55., 55., 56., 56.,
            56., 56., 55., 46., 33., 19.,  8.,  7., 11., 20., 28., 35., 41.,
            46., 51., 55., 59., 60., 62., 64., 66., 67., 68., 68., 68., 68.,
            69., 68., 67., 66., 66., 68., 68., 68., 68., 67., 65., 63., 61.,
            59., 55., 46., 37., 30., 27., 27., 29., 31., 31., 30., 29., 30.,
            32., 33., 33., 35., 35., 35., 34., 33., 34., 37., 39., 43., 45.,
            49., 52., 55., 56., 59., 61., 63., 63., 64., 63., 63., 64., 64.,
            64., 64., 64., 65., 67., 67., 66., 65., 65., 65., 65., 63., 59.,
            51., 39., 27., 19., 13., 11., 12., 19., 25., 30., 36., 41., 45.,
            49., 51., 52., 49., 46., 45., 44., 45., 46., 48., 51., 54., 58.,
            60., 62., 63., 63., 63., 64., 64., 63., 60., 54., 47., 37., 25.,
            12.,  7., 11., 21., 29., 35., 41., 46., 49., 52., 55., 58., 59.,
            59., 61., 62., 62., 61., 58., 52., 46., 38., 30., 19., 18., 22.,
            28., 33., 37., 41., 43., 43., 39., 33., 30., 33., 37., 42., 44.,
            48., 52., 54., 56., 58., 60., 60., 64., 67., 70., 69., 66., 60.,
            56., 51., 40., 25., 17., 18., 23., 29., 34., 40., 45., 50., 55.,
            58., 59., 58., 57., 57., 57., 56., 53., 50., 47., 43., 40., 33.,
            23., 16., 17., 22., 28., 34., 39., 44., 48., 52., 55., 57., 59.,
            60., 62., 63., 63., 62., 59., 53., 44., 34., 23., 16., 17., 23.,
            30., 36., 42., 47., 51., 53., 55., 58., 60., 63., 65., 68., 69.,
            70., 72., 72., 72., 72., 73., 72., 70., 68., 64., 61., 59., 58.,
            58.};


const int N = 2;
const float lambda = 0.99;
const float delta = 0.1; 

float Y[20] = {};
float Y_virtual[20] = {};
float vetor_de_entrada[N] = {0.0, 0.0};
float x_ant[N] = {0.0, 0.0};
float virtual_input[N] = {0.0, 0.0};
float y_pred = 0;
int forecasting = 2;

int FLAG[nA] = {};

RLSFilter filter(lambda, delta);
TEDA teda(1.7);
int N_outlier_max = 3;
bool correction = true;

int flag = 0;
int outlier_count = 0;


const int INTERVALO = 500; // Intervalo de tempo em milissegundos

//Input data
float valor_atual, y_pred_virtual =0;


int main(){
    cout << "flag_cpp = [";
    //cout << "y_pred_cpp = [";
    for (int i=0; i<nA; i++){

        valor_atual = A[i];

        flag = teda.run(valor_atual);

        if(i==0){
            flag=0;
        }

        if (flag == 1){
            outlier_count++;
            valor_atual = y_pred;
        }
        else{
            outlier_count = 0;
        }

        if (correction == true && outlier_count == N_outlier_max + 1)
        {
            valor_atual = A[i];
        }
        
        // Updating weights
        filter.update(valor_atual, x_ant);

        vetor_de_entrada[0] = x_ant[1];
        vetor_de_entrada[1] = valor_atual;
        
        // Predicting
        y_pred = filter.filter(vetor_de_entrada);

        //Virtual filter
        RLSFilter virtual_filter = filter;
        virtual_filter.update(y_pred, vetor_de_entrada);

        virtual_input[0] = vetor_de_entrada[1];
        virtual_input[1] = y_pred;

        for (int j = 0; j < forecasting-1; j++)
        {
            y_pred_virtual = virtual_filter.filter(virtual_input);
            Y_virtual[j] = y_pred_virtual;
            virtual_filter.update(y_pred_virtual, virtual_input);

            virtual_input[0] = virtual_input[1];
            virtual_input[1] = y_pred_virtual;
        }


        // Second stage
        if (flag == 1){
            valor_atual = y_pred;
        }
        // Consecutive outliers treating
        if(correction == true && outlier_count == N_outlier_max + 1){
            outlier_count = 0;
            valor_atual = A[i];
        }

        //cout << (y_pred);
        cout << (flag);
        cout << (", ");
        //Updating previous values vector
        x_ant[0] = vetor_de_entrada[0];
        x_ant[1] = vetor_de_entrada[1]; 
    }
}