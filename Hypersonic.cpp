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
    int owner;
    int row;
    int col;
    int time;
    int range;
};
struct Player
{
    int id;
    int row;
    int col;
    int stock;
    int range;
};
struct Item
{
    int row;
    int col;
    int type;
};
struct Entity
{
    int entity_type;
    int owner;
    int x;
    int y;
    int param_1;
    int param_2;
};

int main()
{
    string experimentBoard[]={
        ".............",
        ".X.........X.",
        "...X.....X...",
        ".0.1.....1.0.",
        ".0..2...2..0.",
        ".0...101...0.",
        ".0..2...2..0.",
        ".0.1.....1.0.",
        "...X.....X...",
        ".X.........X.",
        "............."
    };
    int board[11][13]{};
    int width;
    int height;
    int my_id;
    mt19937 rng(time(NULL));
    // cin>>width>>height>>my_id;
    width=13;
    height=11;
    my_id=0;

    //starting player entities
    Entity starters[]={
        {0,0,0,0,1,3},
        {0,1,12,10,1,3}
    };
    
    
    const int box=0;
    const int box1=1;
    const int box2=2;
    const int floor=-1;
    const int bomb=3;
    const int item=4;
    const int wall=5;
    const int dirs[4][2]{{-1,0},{0,1},{1,0},{0,-1}};
    bool first=true;
    time_t startTime;
    const double calcTime=0.090;
    int turns=200;
    vector<Player> players;
    vector<Bomb> bombs;
    vector<Item> items;
    // game loop
    while (1) {
        for (int i = 0; i < height; i++) {
            string row;
            // cin >> row; cin.ignore();
            row=experimentBoard[i];
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
                    case '2':board[i][j]=box;break;
                    case 'X':board[i][j]=wall;
                }
            }
        }
        int entities=2;
        
        // cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++)
        {
            int entity_type=starters[i].entity_type;
            int owner=starters[i].owner;
            int x=starters[i].x;
            int y=starters[i].y;
            int param_1=starters[i].param_1;
            int param_2=starters[i].param_2;
            // cin >> entity_type >> owner >> x >> y >> param_1 >> param_2; cin.ignore();
            if(entity_type==0)
            {
                players.push_back({owner,y,x,param_1,param_2-1});
                cerr<<"player id "<<owner<<endl;
            }
            else if(entity_type==1)
            {
                bombs.push_back({owner,y,x,param_1,param_2-1});
                board[y][x]=bomb;
            }
            else if(entity_type==2)
            {
                items.push_back({y,x,param_1});
                board[y][x]=item;
            }
            // do bombs ever have 0 turns left? no
            // do bombs ever appear with 8 turns?
            // bombs return to stock on the turn they blow up
        }
        // straight monte carlo sim
        // identify possible moves
        // 0 for move, 1 for bomb
        vector<vector<int>> moves;
        int mCount;
        int curMove=-1;
        int scores[]{};
        int rollouts=0;
        for(Player& me:players)
        {
            if(me.id==my_id)
            {
                moves.push_back({me.row,me.col,0});
                if(me.stock>0) moves.push_back({me.row,me.col,1});
                for(int dir=0;dir<4;dir++)
                {
                    int dr=me.row+dirs[dir][0];
                    int dc=me.col+dirs[dir][1];
                    if(dr>=0 && dr<11 && dc>=0 && dc<13 && (board[dr][dc]==floor || board[dr][dc]==item))
                    {
                        moves.push_back({dr,dc,0});
                        if(me.stock>0)
                            moves.push_back({dr,dc,1});
                    }
                }
                mCount=moves.size();
                break;
            }
        }

        int boxesLeft=0;
        for(int row=0;row<11;row++)
            for(int col=0;col<13;col++)
                if(board[row][col]==box)
                    boxesLeft++;
        int simcounter=0;

        //monte carlo sim loop
        // while(((double)(clock()-startTime)/(double)CLOCKS_PER_SEC)<=calcTime)
        while(simcounter<1000)
        {
            cerr<<"start mc"<<endl;
            first=true;
            // inc curMove
            curMove=(curMove+1)%mCount;

            // create sim state
            vector<Player> sPlayers(players);
            vector<Bomb> sBombs(bombs);
            vector<Item> sItems(items);
            cerr<<"sim vectors allocated"<<endl;
            int sBoxesLeft=boxesLeft;
            int simMap[11][13];
            for(int i=0;i<11;i++)
                copy(board[i],board[i]+13,simMap[i]);
            cerr<<"simMap setup"<<endl;
            int simTurns=turns;
            int score=0;
            // rollout loop
            while(1)
            {
                cerr<<"start rollout"<<endl;
                // decrement bomb timers
                for(Bomb& b:sBombs) b.time--;
                // set off sympathetic bombs
                bool boom=true;
                while(boom)
                {
                    boom=false;
                    for(Bomb& b:sBombs)
                    {
                        if(b.time==0)
                        {
                            boom=true;
                            b.time=-1;
                            int dmax=b.range;
                            for(int dir=0;dir<4;dir++)
                            {
                                int dr=b.row;
                                int dc=b.col;
                                for(int dist=0;dist<dmax;dist++)
                                {
                                    dr+=dirs[dir][0];
                                    dc+=dirs[dir][1];
                                    if(dr>=0 && dr<11 && dc>=0 && dc<13)
                                    {
                                        int here=simMap[dr][dc];
                                        if(here==bomb)
                                        {
                                            for(Bomb& sb:sBombs)
                                            {
                                                if(sb.row==dr && sb.col==dc && sb.time!=-1)
                                                {
                                                    sb.time=0;
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                        else if(here!=floor) break;
                                    }//if in bounds
                                    else break;
                                }//for each dist
                            }//for each dir
                        }//if bomb time is 0
                    }//for each bomb(sympathetic detonations)
                }//while a bomb went off last pass
                // explosions affect objects/players
                vector<vector<int>> dBoxes;// destroyed boxes {row,col,item contained,1 if I tagged it 0 otherwise}
                vector<vector<int>> dItems;// destroyed items {row,col}
                vector<vector<int>> dBombs;// destroyed bombs {row,col,owner}
                cerr<<"d vectors declared"<<endl;
                bool iDied=false;
                bool iWon=false;
                for(Bomb& b:sBombs)
                {
                    if(b.time==-1)
                    {
                        int dmax=b.range;
                        int dr=b.row;
                        int dc=b.col;
                        for(int dir=0;dir<4;dir++)
                        {
                            for(int dist=0;dist<dmax;dist++)
                            {
                                dr+=dirs[dir][0];
                                dc+=dirs[dir][1];
                                if(dr>=0 && dr<11 && dc>=0 && dc<13)
                                {
                                    // check for player kills
                                    int pCount=sPlayers.size();
                                    for(int pInd=pCount-1;pInd>=0;pInd--)
                                    {
                                        Player& p=sPlayers[pInd];
                                        if(dr==p.row && dc==p.col)
                                        {
                                            // if i died, end rollout with 0 score
                                            if(p.id==my_id)
                                            {
                                                iDied=true;
                                                break;
                                            }
                                            else
                                            {
                                                sPlayers.erase(sPlayers.begin()+pInd);
                                                // if only 1 player remaining, take win
                                                if(sPlayers.size()==1)
                                                {
                                                    iWon=true;
                                                    break;
                                                }
                                            }
                                            // else erase player from vector
                                        }//if player hit
                                    }//for each player
                                    int here=simMap[dr][dc];
                                    // check for box hit
                                    if(here==box || here==box1 || here==box2)
                                    {
                                        // if not already added, add
                                        bool present=false;
                                        for(vector<int>& box:dBoxes)
                                        {
                                            if(box[0]==dr && box[1]==dc)
                                            {
                                                if(b.owner==my_id) box[3]=1;
                                                present=true;
                                                break;
                                            }
                                        }
                                        if(!present)
                                            dBoxes.push_back({dr,dc,(here==box?0:here==box1?1:2),(b.owner==my_id?1:0)});
                                        break;
                                    }//if explosion hit a box
                                    else if(here==item)
                                    {
                                        dItems.push_back({dr,dc});
                                    }//if explosion hit an item
                                    else if(here==bomb)
                                    {
                                        // if not already added, add
                                        bool present=false;
                                        for(vector<int>& bomb:dBombs)
                                        {
                                            if(bomb[0]==dr && bomb[1]==dc)
                                            {
                                                present=true;
                                                break;
                                            }
                                        }
                                        if(!present)
                                        {
                                            //find owner
                                            int owner;
                                            for(Bomb& ob:sBombs)
                                                if(ob.row==dr && ob.col==dc)
                                                {
                                                    owner=ob.owner;
                                                }
                                            dBombs.push_back({dr,dc,owner});
                                        }
                                        break;
                                    }//if explosion hit a bomb
                                    else if(here!=floor)
                                    {
                                        break;
                                    }//if explosion hit something else (wall, presumably)
                                }//if in bounds
                            }//for each dist
                        }//for each dir
                    }//if bomb went off
                    if(iDied || iWon) break;
                }//for each bomb(explosion effects)
                if(iWon)
                {
                    scores[curMove]+=score+sBoxesLeft;
                    cerr<<"i won"<<endl;
                    break;
                }
                if(iDied)
                {
                    cerr<<"i died"<<endl;
                    break;
                }
                // destroy boxes
                for(vector<int>& box:dBoxes)
                {
                    score+=box[3];
                    simMap[box[0]][box[1]]=(box[2]==0?floor:item);
                    // add item to item list if one was in the box
                    if(box[2]>0) sItems.push_back({box[0],box[1],box[2]});
                    sBoxesLeft--;
                }
                // destroy items
                for(vector<int>& item:dItems)
                {
                    simMap[item[0]][item[1]]=floor;
                    int iCount=sItems.size();
                    for(int iInd=0;iInd<iCount;iInd++)
                    {
                        Item& i=sItems[iInd];
                        if(i.row==item[0] && i.col==item[1])
                        {
                            sItems.erase(sItems.begin()+iInd);
                        }
                    }
                }
                // destroy bombs
                for(vector<int>& bomb:dBombs)
                {
                    simMap[bomb[0]][bomb[1]]=floor;
                    int bCount=sBombs.size();
                    for(int bInd=0;bInd<bCount;bInd++)
                    {
                        // remove bomb from sBombs
                        Bomb& db=sBombs[bInd];
                        if(db.row==bomb[0] && db.col==bomb[1])
                        {
                            sBombs.erase(sBombs.begin()+bInd);
                            break;
                        }
                    }
                    // return bomb to player's stock
                    for(Player& player:sPlayers)
                    {
                        if(player.id==bomb[2])
                        {
                            player.stock++;
                            break;
                        }
                    }
                }
                //choose and execute a move for each player
                for(Player& p:sPlayers)
                {
                    vector<vector<int>> sMoves;
                    vector<int> move;
                    if(first && p.id==my_id)
                    {
                        first=false;
                        // move=moves[curMove];
                        move=moves.at(curMove);
                    }
                    else
                    {
                        sMoves.push_back({p.row,p.col,0});
                        if(p.stock>0) sMoves.push_back({p.row,p.col,1});
                        for(int dir=0;dir<4;dir++)
                        {
                            int dr=p.row+dirs[dir][0];
                            int dc=p.col+dirs[dir][1];
                            if(dr>=0 && dr<11 && dc>=0 && dc<13 && (simMap[dr][dc]==floor || simMap[dr][dc]==item))
                            {
                                sMoves.push_back({dr,dc,0});
                                if(p.stock>0)
                                    sMoves.push_back({dr,dc,1});
                            }
                        }
                        int sMoveCount=sMoves.size();
                        move=sMoves[rng()%sMoveCount];
                    }
                    if(move[2]==1)
                    {
                        simMap[p.row][p.col]=bomb;
                        sBombs.push_back({p.id,p.row,p.col,8,p.range});
                    }
                    p.row=move[0];
                    p.col=move[1];
                }
                // check for items picked up. type 1 is range, type 2 is stock
                vector<vector<int>> pItems;// picked up items. {row,col}
                for(Player& p:sPlayers)
                {
                    if(simMap[p.row][p.col]==item)
                    {
                        for(Item& i:sItems)
                        {
                            if(i.row==p.row && i.col==p.col)
                            {
                                switch(i.type)
                                {
                                    case 1:p.range++;break;
                                    case 2:p.stock++;
                                }
                                //add to pItems if not already present
                                bool present=false;
                                for(vector<int>& pi:pItems)
                                {
                                    if(i.row==pi[0] && i.col==pi[1])
                                    {
                                        present=true;
                                        break;
                                    }
                                }
                                if(!present) pItems.push_back({i.row,i.col});
                                break;
                            }
                        }
                    }
                }
                // remove picked up items from map/list
                for(vector<int>& pi:pItems)
                {
                    simMap[pi[0]][pi[1]]=floor;
                    int iCount=sItems.size();
                    for(int iInd=0;iInd<iCount;iInd++)
                    {
                        Item& i=sItems[iInd];
                        if(i.row==pi[0] && i.row==pi[1]) sItems.erase(sItems.begin()+iInd);
                    }
                }
                simTurns--;
                if(sBoxesLeft==0 && simTurns>20) simTurns=20;
                if(simTurns==0)
                {
                    scores[curMove]+=score;
                    break;
                }
            }//rollout loop
            rollouts++;
            if(rollouts%10==0) cerr<<rollouts<<endl;
            cerr<<rollouts<<endl;
        }//monte carlo sim loop
        cerr<<"exited MC at "<<((double)(clock()-startTime)/(double)CLOCKS_PER_SEC)<<" s"<<endl;
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
        vector<int>& move=moves[bestInd];
        if(move[2]==1) cout<<"BOMB ";
        else cout<<"MOVE ";
        cout<<move[1]<<" "<<move[0]<<endl;

        first=true;
        turns--;
        return 0;
    }//game loop
}//main()