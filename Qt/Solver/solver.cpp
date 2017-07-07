#include <QDebug>
#include "solver.h"



Solver::Solver()
{
    countCell = 0;
    countPossibility = 0;
    for (int i = 0; i <9; i++){
        for (int j = 0; j <9; j++){
            for (int k = 0; k <9; k++){
                data[i][j][k] = false;
            }
            checked[i][j] = 0;
            nominee[i][j] = 9;
        }
    }
}

void Solver::start()
{
    for (int i = 0; i< 9; i++){
        for (int j = 0; j<9; j++){
            if(checked[i][j]){
                countCell++;
                nominee[i][j] = 0;
                continue;

            }
            else
                for (int n = 0; n <9; n++){
                    data[i][j][n] = true;
                    countPossibility +=1;
                }
        }
    }
}

void Solver::currentValue()
{
    qDebug() << "first";
    for (int i = 0; i < 9; i++){
        int thisrow[9];
        for (int j = 0; j < 9; j++){
            if (checked[i][j]){
                for (int n = 0; n < 9; n++)
                    if (data[i][j][n])
                        thisrow[j] = n+1;
            }
            else thisrow[j] = 0;
        }
        qDebug() <<thisrow[0] <<thisrow[1] <<thisrow[2] <<thisrow[3] <<thisrow[4] <<thisrow[5] <<thisrow[6] <<thisrow[7] <<thisrow[8];
    }
}

bool Solver::checkCell()
{
    bool res;
    for (int i = 0; i < 9; i++ ){
        for (int j = 0; j < 9; j++){
            if (checked[i][j])
                continue;
            else if (nominee[i][j] == 1){
                int k = 0;
                for (; k < 9; k++)
                    if (data[i][j][k]) break;
                checked[i][j] = k;
                countCell++;
                countPossibility--;
                nominee[i][j] =0;
                res = true;
            }
        }
    }
    return res;
}

bool Solver::checkPossible()
{
    for(int i = 0; i < 9; i ++){
        for (int j = 0; j < 9; j++){

            int val = checked[i][j];
            if (val)
            {
                //check in row
                for (int n = 0; n < 9; n++){
                    if (!checked[n][j] && data[n][j][val-1])
                    {
                        nominee[n][j]--;
                        countPossibility--;
                        data[n][j][val-1] = false;
                    }
                }

                //check in column
                for (int n = 0; n < 9; n++){
                    if (!checked[i][n] && data[i][n][val-1])
                    {
                        nominee[i][n]--;
                        countPossibility--;
                        data[i][n][val-1] = false;
                    }
                }

                //check in mini square
                int c = i / 3, r = j / 3;
                for (int a = c*3; a < (c+1)*3; a++){
                    for (int b = r*3; b < (r+1)*3; b++){
                        if (!checked[a][b] && data[a][b][val-1])
                        {
                            nominee[a][b]--;
                            countPossibility--;
                            data[a][b][val-1] = false;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++)
            if (nominee[i][j]==1)
                return true;
    }
    return false;
}

bool Solver::hiddenSingle()
{
    for (int i = 0; i< 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            for (int n = 0; n < 9; n++)
            {
                if (checked[i][j] || !data[i][j][n]) continue;
                int num = 0;
                for (int k = 0; k < 9; k++)
                    if (data[k][j][n])
                        num++;
                if(num==1)
                {
                    for (int m = 0; m < 9; m++){
                        if (m==n || !data[i][j][m]) continue;
                        data[i][j][m] = false;
                        nominee[i][j]--;
                        countPossibility--;
                    }
                }

                num=0;
                for (int k = 0; k < 9; k++)
                    if (data[i][k][n])
                        num++;
                if(num==1)
                {
                    for (int m = 0; m < 9 ; m++){
                        if (m==n || !data[i][j][m]) continue;
                        data[i][j][m] = false;
                        nominee[i][j]--;
                        countPossibility--;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++)
            if (nominee[i][j]==1)
                return true;
    }
    return false;
}

bool Solver::nakedPair()
{

    //check by row


}

