//Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <math.h>
#include <numeric>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include "getopt.h"
#include "P2random.h"
#include <queue>
using namespace std;

class WallSt{
    private: 

        enum class TimeStatus : char{
            NoTrades, // Havent seen a sell order
            CanBuy, //seen a sell order in a specific stock
            Completed, // see a buy order that's a match, if you see a better buy, change it 
            Potential //if a better sell order comes on
        };

        struct traders{
            uint32_t id; 
            uint32_t num_bought = 0;
            uint32_t num_sold = 0; 
            int net_profit = 0; 
        };
        struct Order{
            uint32_t timestamp = 0; 
            uint32_t sequence = 0; 
            uint32_t trader_id; 
            uint32_t price;
            mutable uint32_t quantity;
        };

        struct buyComparator{
            bool operator()(const Order &a, const Order &b) const {
                if(a.price == b.price){
                    return a.sequence > b.sequence;
                }else{return a.price < b.price;}
            }
        };
        struct sellComparator{
            bool operator()(const Order &a, const Order &b) const {
                if(a.price == b.price){
                    return a.sequence > b.sequence;
                }else{return a.price > b.price; }
            }
        };
        struct stocks{
            // max heap
            std::priority_queue<Order, vector<Order>, buyComparator> buyPQ;
            //min heap
            std::priority_queue<Order, vector<Order>, sellComparator> sellPQ;
            //min heap
            std::priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> richStock; 
            //max heap
            std::priority_queue<uint32_t, vector<uint32_t>, less<uint32_t>> brokeStock; 
            //time traveler
            TimeStatus time_state = TimeStatus::NoTrades;
            //uint32
            uint32_t buy_price;
            uint32_t sell_price;
            uint32_t buy_timestamp;
            uint32_t sell_timestamp;
            uint32_t potential_buy_price;
            uint32_t potential_buy_time;

        };
        //struct for median
        /*have upper and lower
        don't need an extra vector of prices
        2 PQs
        lower PQ is a max and upper PQ is a min*/
        
        vector<stocks> TSLA;
        vector<traders>pocketWatch; 
        string input_mode;
        uint32_t num_traders, num_stocks; 
        uint32_t num_orders, arrival_rate, random; 
        uint32_t trades_completed = 0; 
        bool seen1 = false;
        bool seen2 = false;
        bool seen3 = false;
        bool seen4 = false; 
        //two priority queues, one for sell and one for buy
        //greater<int>, lower price seller, less<int> higher price buyer

        void processOrders(istream &inputstream){
            cout << "Processing orders...\n";
            Order order_line; 
            string buy_sell;
            uint32_t idx = 0;
            uint32_t prev_timestamp = 0;
            uint32_t stock_num = 0;
            char junk;
            bool buying = false; 

            if(seen3){
                traders trader;
                pocketWatch.resize(num_traders);
                for(uint32_t j = 0; j < pocketWatch.size(); j++){
                    trader.id = j; 
                    pocketWatch[j] = trader;
                }
            }
            while(inputstream >> order_line.timestamp >> buy_sell >> junk >> order_line.trader_id 
            >> junk >> stock_num >> junk >> order_line.price >> junk >> order_line.quantity){
                order_line.sequence = idx;
                idx++;

                if(order_line.timestamp > prev_timestamp && seen2){
                    for(uint32_t i = 0; i < TSLA.size(); i++){
                        if(!TSLA[i].brokeStock.empty() || !TSLA[i].richStock.empty()){ 
                            uint32_t medianVal = calcMedian(TSLA[i]);
                            cout << "Median match price of Stock " << i << " at time " << 
                            prev_timestamp << " is $" << medianVal << "\n";
                        }
                    }
                }
               
                //Error checking
                if(static_cast<int>(order_line.timestamp) < 0){
                    cerr << "Error: Invalid timestamp\n";
                    exit(1);
                }
                if(prev_timestamp > order_line.timestamp){
                    cerr << "Error: Decreasing timestamp\n";
                    exit(1);
                }else{
                    prev_timestamp = order_line.timestamp;
                }
                if(order_line.trader_id >= num_traders){
                    cerr << "Error: Invalid trader ID\n";
                    exit(1);
                }
                if(stock_num >= num_stocks){
                    cerr << "Error: Invalid stock ID\n";
                    exit(1);
                }
                if(static_cast<int>(order_line.price) < 0){
                    cerr << "Error: Invalid price\n";
                    exit(1);
                }
                if(static_cast<int>(order_line.quantity) < 0){
                    cerr << "Error: Invalid quantity\n";
                    exit(1);
                }

                //processing
                auto &GOOG = TSLA[stock_num]; 
                //make buySell local variables
                if(buy_sell == "BUY"){
                    GOOG.buyPQ.push(order_line);
                    if(seen4){buying = true;}
                }else{
                    GOOG.sellPQ.push(order_line);
                    if(seen4){buying = false; }
                }

                while(!GOOG.buyPQ.empty() && !GOOG.sellPQ.empty() && 
                (GOOG.buyPQ.top().price >= GOOG.sellPQ.top().price)){
                    uint32_t min_quantity = static_cast<uint32_t>(min(GOOG.buyPQ.top().quantity, GOOG.sellPQ.top().quantity));
                    uint32_t transaction_amount = 0; 
                    if((GOOG.buyPQ.top().sequence < GOOG.sellPQ.top().sequence) && 
                    GOOG.buyPQ.top().price >= GOOG.sellPQ.top().price){
                        transaction_amount = static_cast<uint32_t>(GOOG.buyPQ.top().price);
                    }
                    else if((GOOG.sellPQ.top().sequence < GOOG.buyPQ.top().sequence) && 
                    (GOOG.sellPQ.top().price <= GOOG.buyPQ.top().price)){
                        transaction_amount = static_cast<uint32_t>(GOOG.sellPQ.top().price);
                    }

                    if(seen1){
                        cout << "Trader " << GOOG.buyPQ.top().trader_id << " purchased " << min_quantity << " shares of Stock " <<
                        stock_num << " from Trader " << GOOG.sellPQ.top().trader_id << " for $" << transaction_amount << "/share\n";
                    }//if seen1

                    if(seen2){
                        if(GOOG.brokeStock.empty()){
                            GOOG.brokeStock.push(transaction_amount);
                        }else if(transaction_amount > GOOG.brokeStock.top()){
                            GOOG.richStock.push(transaction_amount);
                        }else{
                            GOOG.brokeStock.push(transaction_amount);
                        }
                    }//seen2
                    if(seen3){
                        pocketWatch[GOOG.buyPQ.top().trader_id].num_bought += min_quantity;
                        pocketWatch[GOOG.sellPQ.top().trader_id].num_sold += min_quantity;
                        
                        int total_net = static_cast<int>(min_quantity * transaction_amount); 
                        pocketWatch[GOOG.buyPQ.top().trader_id].net_profit -= total_net;
                        pocketWatch[GOOG.sellPQ.top().trader_id].net_profit += total_net;
                    }//seen3

                    //update quantity and pop PQs
                    if(GOOG.buyPQ.top().quantity == min_quantity){
                        GOOG.buyPQ.pop(); 
                    }else{
                        GOOG.buyPQ.top().quantity -= min_quantity;
                    }
                    if(GOOG.sellPQ.top().quantity == min_quantity){
                        GOOG.sellPQ.pop(); 
                    }else{
                        GOOG.sellPQ.top().quantity -= min_quantity;
                    }
                    trades_completed++;
                }//while match
                if(seen4){
                    //timetraveler, pass in the order, bool bu
                    calcTimeTraveler(GOOG, order_line, buying);
                }
            }//while cin

            if(seen2){
            //last median output after reading in 
                for(uint32_t i = 0; i < TSLA.size(); i++){
                    if(!TSLA[i].brokeStock.empty() || !TSLA[i].richStock.empty()){ 
                        uint32_t medianVal = calcMedian(TSLA[i]);
                        cout << "Median match price of Stock " << i << " at time " << 
                        prev_timestamp << " is $" << medianVal << "\n";
                    }
                }//for
            }//seen
       }//process orders
       
       uint32_t calcMedian(stocks &t){
        while((t.brokeStock.size() > t.richStock.size()) && 
        (t.brokeStock.size() - t.richStock.size() != 1)){
            t.richStock.push(t.brokeStock.top());
            t.brokeStock.pop();
        }//while
        while((t.brokeStock.size() < t.richStock.size()) && 
        (t.richStock.size() - t.brokeStock.size() != 1)){
            t.brokeStock.push(t.richStock.top());
            t.richStock.pop();
        }//while
        if(t.richStock.size() < t.brokeStock.size()){
            return t.brokeStock.top();
        } else if(t.richStock.size() > t.brokeStock.size()){
            return t.richStock.top();
        } else{
            return (t.brokeStock.top() + t.richStock.top()) / 2; }
       }//calcMedian

       void calcTimeTraveler(stocks &GOOG, Order &order, bool &buy){
            if(GOOG.time_state == TimeStatus::NoTrades){
                if(buy){
                    return; 
                }
                else if(!buy){
                    GOOG.buy_price = order.price;
                    GOOG.buy_timestamp = order.timestamp;
                    GOOG.time_state = TimeStatus::CanBuy;
                }
            }
            else if(GOOG.time_state == TimeStatus::CanBuy){
                if(buy){
                    if(order.price > GOOG.buy_price){
                        GOOG.sell_price = order.price; 
                        GOOG.sell_timestamp = order.timestamp;
                        GOOG.time_state = TimeStatus::Completed;
                    }
                }
                else if(!buy){
                    if(order.price < GOOG.buy_price){
                        GOOG.buy_price = order.price; 
                        GOOG.buy_timestamp = order.timestamp; 
                    }
                }
            }
            else if(GOOG.time_state == TimeStatus::Completed){
                if(buy){
                    if(order.price > GOOG.sell_price){
                        GOOG.sell_price = order.price; 
                        GOOG.sell_timestamp = order.timestamp; 
                    }
                }
                else if(!buy){
                    if(order.price < GOOG.buy_price){
                        GOOG.potential_buy_price = order.price; 
                        GOOG.potential_buy_time = order.timestamp; 
                        GOOG.time_state = TimeStatus::Potential; 
                    }
                }
            }
            else if(GOOG.time_state == TimeStatus::Potential){
                if(buy){
                    if((static_cast<int>(order.price - GOOG.potential_buy_price)) > (static_cast<int>(GOOG.sell_price - GOOG.buy_price))){
                        GOOG.sell_price = order.price;
                        GOOG.sell_timestamp = order.timestamp;
                        GOOG.buy_price = GOOG.potential_buy_price;
                        GOOG.buy_timestamp = GOOG.potential_buy_time;
                        GOOG.time_state = TimeStatus::Completed;
                    }
                }
                else if(!buy){
                    if(order.price < GOOG.potential_buy_price){
                        GOOG.potential_buy_price = order.price; 
                        GOOG.potential_buy_time = order.timestamp;
                    }
                }
            }
       }//calcTime

    public:
        void get_options(int argc, char** argv){
            int option_index = 0; 
            int option = 0; 
            opterr = false; 

            struct option longOpts[] = {
                {"verbose", no_argument, nullptr, 'v'},
                {"median", no_argument, nullptr, 'm'},
                {"trader_info", no_argument, nullptr, 'i'},
                {"time_travelers", no_argument, nullptr, 't'},
                {nullptr, 0, nullptr, '\0'}
            };//struct option
            while((option = getopt_long(argc, argv, "vmit", longOpts, &option_index)) != -1){
                switch(option){
                    case 'v':
                        if(seen1){
                            cerr << "Error: Unknown command line option\n";
                            exit(1);
                        }
                        seen1 = true; 
                        break;
                        //verbose = 'v';
                    case 'm':
                        if(seen2){
                            cerr << "Error: Unknown command line option\n";
                            exit(1);
                        }
                        seen2 = true; 
                        break;
                        //median = 'm';
                    case 'i':
                        if(seen3){
                            cerr << "Error: Unknown command line option\n";
                            exit(1);
                        }
                        seen3 = true;
                        break; 
                        //trader_info = 'i';
                    case 't':
                        if(seen4){
                            cerr << "Error: Unknown command line option\n";
                            exit(1);
                        }
                        seen4 = true; 
                        break;
                        //tt = 't'; 
                }//switch
            }//while
        }//get_options

        void read_input(){
            string junk;
            getline(cin, junk);
            cin >> junk >> input_mode >> junk >> num_traders >> junk >> num_stocks;
            TSLA.resize(num_stocks);
            stringstream ss;
            if(input_mode == "PR"){
                cin >> junk >> random >> junk >> num_orders >> junk >> arrival_rate;
                P2random::PR_init(ss, static_cast<unsigned int>(random), static_cast<unsigned int>(num_traders), static_cast<unsigned int>(num_stocks), static_cast<unsigned int>(num_orders), static_cast<unsigned int>(arrival_rate));
            } 
            if(input_mode == "PR"){
                processOrders(ss);
            }else{processOrders(cin);}
        }//readinput 

        void print(){
            cout << "---End of Day---\n";
            cout << "Trades Completed: " << trades_completed << "\n";
            if(seen3){
                cout << "---Trader Info---\n";
                for(size_t i = 0; i < pocketWatch.size(); i++){
                    cout << "Trader " << pocketWatch[i].id << " bought " << pocketWatch[i].num_bought 
                    << " and sold " << pocketWatch[i].num_sold << " for a net transfer of $" 
                    << pocketWatch[i].net_profit << "\n";
                }
            }//seen 3
            if(seen4){
                cout << "---Time Travelers---\n";
                for(size_t i = 0; i < TSLA.size(); i++){
                    if(TSLA[i].time_state == TimeStatus::Potential || TSLA[i].time_state == TimeStatus::Completed){
                        cout << "A time traveler would buy Stock " << i <<
                        " at time " << TSLA[i].buy_timestamp << " for $" << TSLA[i].buy_price <<
                        " and sell it at time " << TSLA[i].sell_timestamp << " for $" << TSLA[i].sell_price
                        << "\n";
                    }else{
                        cout << "A time traveler could not make a profit on Stock " << i << "\n";
                    }
                }//for
            }//seen4
        }//print
};//WallSt

int main(int argc, char** argv){
    std::ios::sync_with_stdio(false);
    WallSt Wolf; 
    Wolf.get_options(argc, argv);
    Wolf.read_input();
    Wolf.print();
    return 0; 
}//main
