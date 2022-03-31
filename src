#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <time.h>
#include <random>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
struct Bomb
{
    int row;
    int col;
    int range;
    int time;
    int owner;
};
struct Player
{
    int col;
    int row;
    int stock;
};
int main()
{
    int board[11][13]{};
    int width;
    int height;
    int my_id;
    mt19937 rng(time(NULL));
    cin>>width>>height>>my_id;
    
    const int box=20;
    const int deadBox=21;
    const int floor=-1;
    const int bomb=2;
    const int dirs[4][2]{{-1,0},{0,1},{1,0},{0,-1}};
    bool first=true;
    time_t startTime;
    const double calcTime=0.090;
    int turns=200;
    Player me,opp;

    // game loop
    while (1) {
        for (int i = 0; i < height; i++) {
            string row;
            cin >> row; cin.ignore();
            if(first)
            {
                startTime=clock();
                first=false;
            }
            for(int j=0;j<13;j++)
            {
                char here=row[j];
                
                switch(here)
                {
                    case '.':board[i][j]=floor;break;
                    case '0':
                    case '1':
                    case '2':board[i][j]=box;
                }
            }
        }
        vector<Bomb> bombs;
        int entities;
        int r,c,stock,range;
        
        cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++)
        {
            int entity_type;
            int owner;
            int x;
            int y;
            int param_1;
            int param_2;
            cin >> entity_type >> owner >> x >> y >> param_1 >> param_2; cin.ignore();
            if(entity_type==0)
            {
                if(owner==my_id)
                {
                    me={x,y,param_1};
                    r=y;
                    c=x;
                    stock=param_1;
                    range=param_2-1;
                }
                else
                    opp={x,y,param_1};
            }
            if(entity_type==1)
            {
                bombs.push_back({y,x,param_2-1,param_1,owner});
                board[y][x]=bomb;
            }
            // do bombs ever have 0 turns left? no
            // bombs return to stock on the turn they blow up
        }
        // straight monte carlo sim
        // identify possible moves
        // 0 for move, 1 for bomb
        vector<vector<int>> moves;
        for(int dir=0;dir<4;dir++)
        {
            moves.push_back({me.row,me.col,0});
            if(me.stock>0) moves.push_back({me.row,me.col,1});
            int dr=me.row+dirs[dir][0];
            int dc=me.col+dirs[dir][1];
            if(dr>=0 && dr<11 && dc>=0 && dc<13 && (board[dr][dc]==floor))
            {
                moves.push_back({dr,dc,0});
                if(me.stock>0)
                    moves.push_back({dr,dc,1});
            }
        }
        int mCount=moves.size();
        int curMove=-1;
        int scores[10]{};
        int rollouts=0;

        //monte carlo sim loop
        while(((double)(clock()-startTime)/(double)CLOCKS_PER_SEC)<=calcTime)
        {
            first=true;
            // inc curMove
            curMove=(curMove+1)%mCount;
            // create sim state
            Player simMe{me.col,me.row,me.stock};
            Player simOpp{opp.col,opp.row,opp.stock};
            int simMap[11][13];
            for(int i=0;i<11;i++)
                copy(board[i],board[i]+13,simMap[i]);
            vector<Bomb> simBombs;
            for(Bomb b:bombs)
            {
                simBombs.push_back({b.row,b.col,b.range,b.time,b.owner});
                simMap[b.row][b.col]=bomb;
            }
            int simTurns=turns;
            int score=0;
            int boxesLeft=0;
            for(int row=0;row<11;row++)
                for(int col=0;col<13;col++)
                    if(simMap[row][col]==box)
                        boxesLeft++;
            vector<int> myMove=moves[curMove];
            vector<int> oMove;
            // rollout loop
            cerr<<endl<<"new rollout"<<endl;
            while(1)
            {
                // decrement bomb timers
                // go through bombs looking for timer 0
                // if 0, check explosion effects
                    // mark hit bombs time 0 if not already time -1
                    // score for hit boxes
                    // mark this bomb time -1
                    // add hit boxes to destroy list
                bool exploded=true;
                for(Bomb b:simBombs) b.time--;
                vector<vector<int>> destroyedBoxes;
                while(exploded)
                {
                    exploded=false;
                    for(Bomb b:simBombs)
                    {
                        if(b.time==0)
                        {
                            b.time=-1;
                            exploded=true;
                            for(int dir=0;dir<4;dir++)
                            {
                                int dr=b.row;
                                int dc=b.col;
                                for(int dist=0;dist<2;dist++)
                                {
                                    dr+=dirs[dir][0];
                                    dc+=dirs[dir][1];
                                    if(dr>=0 && dr<11 && dc>=0 && dc<13)
                                    {
                                        if(simMap[dr][dc]==bomb)
                                        {
                                            for(Bomb hitBomb:simBombs)
                                            {
                                                if(hitBomb.row==dr && hitBomb.col==dc && hitBomb.time>0)
                                                {
                                                    hitBomb.time=0;
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                        else if(simMap[dr][dc]==box)
                                        {
                                            // if box is not on destroyed list, add it and score if appropriate
                                            bool onlist=false;
                                            for(vector<int> box:destroyedBoxes)
                                            {
                                                if(box[0]==dr && box[1]==dc)
                                                {
                                                    onlist=true;
                                                    break;
                                                }
                                            }
                                            if(!onlist)
                                            {
                                                destroyedBoxes.push_back({dr,dc});
                                                if(b.owner==my_id) score++;
                                            }
                                            break;
                                        }
                                    }
                                    else break;
                                }//for each dist
                            }//for each dir
                        }//if bomb time is 0
                    }//for each bomb
                }//while a bomb exploded last iteration
                // remove time -1 bombs from bomb list
                for(int i=simBombs.size()-1;i>=0;i--)
                {
                    Bomb b=simBombs[i];
                    if(b.time==-1)
                    {
                        if(b.owner==my_id)
                            simMe.stock++;
                        else
                            simOpp.stock++;
                        simMap[b.row][b.col]=floor;
                        simBombs.erase(simBombs.begin()+i);
                    }
                }
                // remove exploded boxes from map
                for(vector<int> boxloc:destroyedBoxes)
                {
                    simMap[boxloc[0]][boxloc[1]]=floor;
                    boxesLeft--;
                }
                // execute orders
                // if not first turn, randomly choose my move
                if(first)
                    first=false;
                else
                {
                    vector<vector<int>> sMoves;
                    sMoves.push_back({simMe.row,simMe.col,0});
                    if(simMe.stock>0) sMoves.push_back({simMe.row,simMe.col,1});
                    for(int dir=0;dir<4;dir++)
                    {
                        int dr=simMe.row+dirs[dir][0];
                        int dc=simMe.col+dirs[dir][1];
                        if(dr>=0 && dr<11 && dc>=0 && dc<13 && (simMap[dr][dc]==floor))
                        {
                            sMoves.push_back({dr,dc,0});
                            if(simMe.stock>0)
                                sMoves.push_back({dr,dc,1});
                        }
                    }
                    int sMoveCount=sMoves.size();
                    cerr<<"my move count "<<sMoves.size()<<endl;
                    myMove=sMoves[rng()%sMoveCount];
                }//choose random move for me
                // randomly choose enemy move
                {
                    vector<vector<int>> sMoves;
                    sMoves.push_back({simOpp.row,simOpp.col,0});
                    if(simOpp.stock>0) sMoves.push_back({simOpp.row,simOpp.col,1});
                    for(int dir=0;dir<4;dir++)
                    {
                        int dr=simOpp.row+dirs[dir][0];
                        int dc=simOpp.col+dirs[dir][1];
                        if(dr>=0 && dr<11 && dc>=0 && dc<13 && simMap[dr][dc]==floor)
                        {
                            sMoves.push_back({dr,dc,0});
                            if(simOpp.stock>0)
                                sMoves.push_back({dr,dc,1});
                        }
                    }
                    int sMoveCount=sMoves.size();
                    cerr<<"opp move count "<<sMoves.size()<<endl;
                    oMove=sMoves[rng()%sMoveCount];
                }
                // draw map
                for(int row=0;row<11;row++)
                {
                    for(int col=0;col<13;col++)
                    {
                        int here=simMap[row][col];
                        bool bombed=false;
                        for(Bomb b:simBombs)
                        {
                            if(b.row==row && b.col==col)
                            {
                                cerr<<b.time;
                                bombed=true;
                                break;
                            }
                        }
                        if(simMe.row==row && simMe.col==col) cerr<<"X";
                        else if(simOpp.row==row && simOpp.col==col) cerr<<"O";
                        else if(!bombed)
                            switch(here)
                            {
                                case floor:cerr<<".";break;
                                case bomb:cerr<<"b";break;
                                case box:cerr<<"%";
                            }
                    }
                    cerr<<endl;
                }
                cerr<<"Me "<<(myMove[2]==1?"BOMB ":"MOVE ")<<myMove[1]<<" "<<myMove[0]<<endl;
                cerr<<"Opp "<<(oMove[2]==1?"BOMB ":"MOVE ")<<oMove[1]<<" "<<oMove[0]<<endl;
                // drop new bombs
                if(myMove[2]==1)
                {
                    simMap[simMe.row][simMe.col]=bomb;
                    simBombs.push_back({simMe.row,simMe.col,2,8,my_id});
                    simMe.stock--;
                }
                if(oMove[2]==1)
                {
                    simMap[simOpp.row][simOpp.col]=bomb;
                    simBombs.push_back({simOpp.row,simOpp.col,2,8,(my_id==1?0:1)});
                    simOpp.stock--;
                }
                // update positions
                simMe.row=myMove[0];
                simMe.col=myMove[1];
                simOpp.row=oMove[0];
                simOpp.col=oMove[1];

                simTurns--;
                if(simTurns==0 || boxesLeft==0)
                {
                    scores[curMove]+=score;
                    break;
                }
            }//rollout
            simBombs.clear();
            rollouts++;
        }//monte carlo sim loop
        for(int i=0;i<mCount;i++) cerr<<scores[i]<<" ";
        cerr<<endl;
        cerr<<"rollouts "<<rollouts<<endl;

        // find highest scoring move
        int best=-1;
        int bestInd=0;
        for(int i=0;i<mCount;i++)
        {
            if(scores[i]>best)
            {
                best=scores[i];
                bestInd=i;
            }
        }
        vector<int> move=moves[bestInd];
        if(move[2]==1) cout<<"BOMB ";
        else cout<<"MOVE ";
        cout<<move[1]<<" "<<move[0]<<endl;

        first=true;
        turns--;
    }//game loop
}//main()
