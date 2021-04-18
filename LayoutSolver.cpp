#include <bits/stdc++.h>

using namespace std;


int xDimension = 1000;
int yDimension = 1000;


double randZeroToOne(){
      return rand() / (RAND_MAX + 1.);
}

double randLessThan(int x){
    return randZeroToOne()*x;
}
double distance(int x1, int y1, int x2, int y2){
    return  sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}

class Label{
    public:
    double leaderLineLength;
    int leaderLineAngle;

    Label(){}
    Label(double leaderLineLength, int leaderLineAngle){
        this->leaderLineLength = leaderLineLength;
        this->leaderLineAngle = leaderLineAngle; 
    }
};

class POI{
    public:
    int x,y;
    int preferredAngleOfLeader;
    string labelText;
    Label label;
    int labelHeight;
    int labelWidth;
    POI(int x, int y, int preferredAngleOfLeader, string labelText, int labelHeight, int labelWidth){
        this->x = x;
        this->y = y;
        this->preferredAngleOfLeader = preferredAngleOfLeader;
        this->labelText = labelText;
        this->labelHeight = labelHeight;
        this->labelWidth = labelWidth;
    }
};

void printLabels(vector <vector <int> > labelMap,vector <POI> &points){
    int i,j;
    cout<<"The labelMap is as follows:"<<endl;
    for(i=0;i<points.size();i++){
        labelMap[points[i].y][points[i].x] = 9;
    }
    for(i=0;i<labelMap.size();i++){
        for(j=0;j<labelMap[0].size();j++){
            cout<<labelMap[i][j]<<" ";
        }
        cout<<endl;
    }
}

class LayoutSolver{
    public:
    int subPenaltyFunc(vector <vector <int> > &spaceArray, int x, int y, POI p, int theta){
        int penalty = 0;
        int width,height;
        width = p.labelWidth;
        height = p.labelHeight;
        int r, c;

        if(theta <= 90){
            for(r = x; r<x+p.labelWidth;r++){
                for(c = y;c>y-p.labelHeight;c--){
                    penalty+= spaceArray[c][r];
                }
            }
        }
        else if(theta <= 180){
            for(r = x; r>x-p.labelWidth;r--){
                for(c = y;c>y-p.labelHeight;c--){
                    penalty+= spaceArray[c][r];
                }
            }
        }
        else if(theta <=270){
            for(r = x; r>x-p.labelWidth;r--){
                for(c = y;c<y+p.labelHeight;c++){
                    penalty+= spaceArray[c][r];
                }
            }
        }
        else{
            for(r = x; r<x+p.labelWidth;r++){
                for(c = y;c<y+p.labelHeight;c++){
                    penalty+= spaceArray[c][r];
                }
            }
        }
        return penalty;
    }
    double penaltyFunc(vector <vector <int> > &importanceMap, vector <vector <int> > &edgeMap,  vector <vector <int> > &labelMap, POI P,int labelX, int labelY, int theta, vector <int> &parameters){
        double penalty;

        double importancePenalty = subPenaltyFunc(importanceMap,labelX,labelY,P, theta);
        double edgePenalty = subPenaltyFunc(edgeMap,labelX,labelY,P,theta);
        double labelPenalty = subPenaltyFunc(labelMap,labelX,labelY,P,theta);
        double leaderLineLength = distance(labelX,labelY,P.x,P.y);
        double anglePenalty = abs(theta - P.preferredAngleOfLeader)/180;
        // cout<<importancePenalty<<" "<<edgePenalty<<" "<<labelPenalty<<" "<<leaderLineLength<<" "<<anglePenalty<<endl;
        penalty = importancePenalty*parameters[0] + edgePenalty*parameters[1] + labelPenalty*parameters[2] + leaderLineLength*parameters[3] + anglePenalty*parameters[4];
       // cout<<"P:"<<penalty<<endl;
        return penalty;
    }
    void run(vector <vector <int> > &importanceMap, vector <vector <int> > &edgeMap, vector <POI> &points, vector <int> &parameters ){
        int xDimension, yDimension;
        xDimension = importanceMap[0].size();
        yDimension = importanceMap.size();
        srand(time(0));
        vector <vector <int> > labelMap(yDimension, vector<int> (xDimension,0));
        vector <Label> labels;

        int i,j;
        int R = parameters[5];
        int searchSpaceSize = parameters[6];
        double sumPenalty = 0;
        
        // boundary conditions
        for(i = 0; i<points.size();i++){
            Label * optimalLabel;

            double optimalPenalty = INT_MAX;
            int labelX,labelY,theta,optimalLabelX,optimalLabelY,optimalLabelTheta;
            for(j = 0; j<searchSpaceSize; j++){
                int r = randLessThan(R);
                theta = randLessThan(360);
                labelX = points[i].x+ r * cos( ((float)theta)/180 * 22.0/7.0);
                labelY = points[i].y+r * sin(((float)theta)/180 * 22.0/7.0);
                if(labelX>= xDimension || labelY>=yDimension || labelX<0 || labelY<0){
                    j--;
                    continue;
                }


                if(theta <= 90){
                    if(labelX+points[i].labelWidth>=xDimension || labelY-points[i].labelHeight<0){
                        continue;
                    }
                }
                else if(theta <= 180){
                    if(labelX-points[i].labelWidth<0 || labelY-points[i].labelHeight<0){
                        continue;
                    }

                }
                else if(theta <=270){
                    if(labelX-points[i].labelWidth<0 || labelY+points[i].labelHeight>=yDimension){
                        continue;
                    }
                }
                else{
                    if(labelX+points[i].labelWidth>=xDimension || labelY+points[i].labelHeight>=yDimension){
                        continue;
                    }

                }

                double penalty = penaltyFunc(importanceMap,edgeMap,labelMap,points[i],labelX,labelY,theta,parameters);
                
                if(penalty < optimalPenalty){
                    optimalLabel = new Label(distance(labelX,labelY,points[i].x, points[i].y), theta ); 
                    optimalPenalty = penalty;
                    optimalLabelX = labelX;
                    optimalLabelY = labelY;
                    optimalLabelTheta = theta;
                }
            }
            points[i].label = *optimalLabel;
          //  cout<<"OP:"<<optimalPenalty<<endl;
            sumPenalty = sumPenalty+optimalPenalty;
            // update labelMap

            int r,c;
            
            if(optimalLabelTheta <= 90){
                for(r = optimalLabelX; r<optimalLabelX+points[i].labelWidth;r++){
                    for(c = optimalLabelY;c>optimalLabelY-points[i].labelHeight;c--){
                        labelMap[c][r]++;
                    }
                }
            }
            else if(optimalLabelTheta <= 180){
                for(r = optimalLabelX; r>optimalLabelX-points[i].labelWidth;r--){
                    for(c = optimalLabelY;c>optimalLabelY-points[i].labelHeight;c--){
                        labelMap[c][r]++;
                    }
                }

            }
            else if(optimalLabelTheta <=270){
                for(r = optimalLabelX; r>optimalLabelX-points[i].labelWidth;r--){
                    for(c = optimalLabelY;c<optimalLabelY+points[i].labelHeight;c++){
                        labelMap[c][r]++;
                    }
                }
            }
            else{
                for(r = optimalLabelX; r<optimalLabelX+points[i].labelWidth;r++){
                    for(c = optimalLabelY;c<optimalLabelY+points[i].labelHeight;c++){
                        labelMap[c][r]++;
                    }
                }

            }
        }
        cout<<"Penalty:"<<sumPenalty<<endl;
        printLabels(labelMap,points);
        
    }
    
};


int main(){
    string myText;
    ifstream MyReadFile("test2");
    int counter = 0;
    vector <vector <int> > importanceMap, edgeMap;
    vector <POI> points;
    vector <int> params;
    while (getline (MyReadFile, myText)) {

        if(myText == ""){
           // cout<<endl<<"---"<<endl;
            counter++;
        }
        else{
            istringstream ss(myText);
            string word;
            vector<int> temp;
            if(counter == 0){
                while (ss >> word){
                // cout<<word<<",";
                    temp.push_back(stod(word));
                }
                edgeMap.push_back(temp);    
            }
            else if(counter == 1){
                //cout<<"---"<<endl;
                while (ss >> word){
                // cout<<word<<",";
                    temp.push_back(stoi(word));
                }
                importanceMap.push_back(temp);
            }
            else if(counter == 2){
            //  cout<<"---"<<endl;
                int x,y,labelWidth, labelHeight;
                string labelText;
                int i = 0;
                while (ss >> word){
                    if(i==0){
                        x = stoi(word);
                    }
                    else if(i==1){
                        y = stoi(word);
                    }
                    else if(i==2){
                        labelText = word;
                    }
                    else if(i==3){
                        labelWidth = stoi(word);
                    }
                    else{
                        labelHeight = stoi(word);
                    }
                    i++;
                }

                POI p(x,y,0,labelText,labelHeight,labelWidth);
                points.push_back(p);
            }
            else{
            // cout<<"---"<<endl;
                while (ss >> word){
                    params.push_back(stoi(word));
                }
            }
        }
        
        // cout<<endl;    
    }    
    xDimension = importanceMap[0].size();
    yDimension = importanceMap.size();
    cout<<"Dimensionality:"<<xDimension<<" "<<yDimension<<endl;
    cout<<"Parameters:"<<params[0]<<" "<<params[1]<<" "<<params[2]<<" "<<params[3]<<" "<<params[4]<<" "<<params[5]<<endl;
    LayoutSolver layoutSolver;
    layoutSolver.run(importanceMap,edgeMap,points,params);
    return 0;
}