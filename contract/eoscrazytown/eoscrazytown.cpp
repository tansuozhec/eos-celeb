#include "eoscrazytown.hpp"

#include <eosiolib/crypto.h>

// @abi action
void eoscrazytown::init(const checksum256 &hash) {
    require_auth(_self);
    /*
    uint64_t _totalPrice = 0;
    for (int i = 0; i < 150; ++i) {
        auto itr = bags.find(i);
        uint64_t _price = itr->price;
        _totalPrice += _price;
    }
    uint64_t amount = 3000 * 10000;
    for (int k = 50; k < 150; ++k) {
        auto itr = bags.find(k);
        uint64_t _price = itr->price;
        account_name _owner = itr->owner;
        uint64_t _amountCMU = _price * amount / _totalPrice;
        action( // winner winner chicken dinner
            permission_level{_self, N(active)},
            N(cryptomeetup), N(airdrop),
            make_tuple(_owner, _amountCMU)
        ).send();    
    }*/

  //  _global.remove();
    auto g = _global.get();
  //  g.earnings_per_share = 0;
//    _global.set(g, _self);   

    singleton_voters _v1(_self, N(thinksaturna));
    auto v1 = _v1.get();
    singleton_voters _v2(_self, N(chenkaioneos));
    auto v2 = _v2.get();
    singleton_voters _v3(_self, N(chenlei33333));
    auto v3 = _v3.get();
    singleton_voters _v4(_self, N(eosotcbackup));
    auto v4 = _v4.get();        
    singleton_voters _v5(_self, N(huaeoshuaeos));
    auto v5 = _v5.get();
    singleton_voters _v6(_self, N(laowantong11));
    auto v6 = _v6.get();   
    singleton_voters _v7(_self, N(lunaaikitoni));
    auto v7 = _v7.get();         


    v1.payout = 0;
    v2.payout = 0;
    v3.payout = 0;
    v4.payout = 0;
    v5.payout = 0;
    v6.payout = 0;
    v7.payout = 0;
    _v1.set(v1,_self);
    _v2.set(v2,_self);
    _v3.set(v3,_self);
    _v4.set(v4,_self);
    _v5.set(v5,_self);
    _v6.set(v6,_self);
    _v7.set(v7,_self);



    eosio_assert(g.total_staked == v1.staked + v2.staked + v3.staked + v4.staked + v5.staked + v6.staked + v7.staked, "wtf");
    g.earnings_per_share = 0;
    _global.set(g, _self);   
}
// @abi action
void eoscrazytown::clear()
{
    require_auth(_self);

    //_global.remove();

    _bagsglobal.remove();

    // multi_index can't erase when the format changed

    /*
        auto it = db_lowerbound_i64(_self, _self, N(global), 0);
        while (it >= 0) {
            auto del = it;
            uint64_t dummy;
            it = db_next_i64(it, &dummy);
            db_remove_i64(del);
}*/

    //while (players.begin() != players.end()) {
    //    players.erase(players.begin());
    // }



}
// @abi action
void eoscrazytown::test() {
}

void eoscrazytown::make_profit(uint128_t amount) {
    auto g = _global.get();
    g.earnings_per_share += MAGNITUDE * amount / g.total_staked;
    _global.set(g, _self);
}

void eoscrazytown::claim(account_name from) {
    require_auth(from);

    singleton_voters _voters(_self, from);
    auto v = _voters.get_or_create(_self, voter_info{});
    auto g = _global.get();        
    // TODO(minakokojima): unvote(v);
    uint64_t delta = 0;
    if (g.earnings_per_share * v.staked / MAGNITUDE <= v.payout) {
        delta = 0;
    } else {
        delta = g.earnings_per_share * v.staked / MAGNITUDE - v.payout;
    }
    v.payout = g.earnings_per_share * v.staked / MAGNITUDE;
    _voters.set(v, _self);    
    
    //singleton_players _players(_self, from);
    //auto p = _players.get_or_create(_self, player_info{});

    if (delta > 0) {
        send_defer_action(
            permission_level{_self, N(active)},
            N(eosio.token), N(transfer),
            make_tuple(_self, from, 
                asset(delta, EOS_SYMBOL),
                string("claim dividend.")
            )
        );
    }

    // p.pool_profit = 0;
    // _players.set(p, _self);   
}

void eoscrazytown::unstake(account_name from, asset quantity) {
    require_auth(from);
    claim(from);
    council::unstake(from, quantity.amount);
    auto g = _global.get();
    g.total_staked -= quantity.amount;
    _global.set(g, _self);      
}

void eoscrazytown::newbag(account_name &from, asset &eos)
{

    require_auth(_self);
    for (int i=0;i<10;++i){
    bags.emplace(from, [&](auto &p) {
        p.id = bags.available_primary_key();
        p.owner = from;
        p.price = eos.amount;
    });
    }
}

void eoscrazytown::setslogan(account_name &from, uint64_t id, string memo)
{
    auto itr = bags.find(id);
    eosio_assert(itr != bags.end(), "no character exist");
    eosio_assert(memo.size() <= 64, "too long");
    eosio_assert(from == itr->owner, "not the owner.");
    bags.modify(itr, from, [&](auto &t) {
        t.slogan = memo;
    });
}

auto eoscrazytown::checkBets(const asset &eos, const string &memo,
                             vector<int64_t> &vbets, int64_t &totalBets)
{ // check eos.amount == total bets
    vbets = getBets(memo, ',');
    totalBets = getTotalBets(vbets);
    return eos.amount == totalBets;
}

// input
/*
void eoscrazytown::onTransfer(account_name &from, account_name &to, asset &eos, string &memo) {        
    if (to != _self) return ;

    require_auth(from);
    eosio_assert(eos.is_valid(), "Invalid token transfer...");
    eosio_assert(eos.symbol == EOS_SYMBOL, "only EOS token is allowed");
    eosio_assert(eos.amount > 0, "must bet a positive amount");
    eosio_assert(memo != "" , "must have bets in memo") ;
    eosio_assert(memo.size() >= 21  , "bets wrong...") ;

                
    // todo: input check non-num
    vector<int64_t> vbets ;
    int64_t totalBets = 0 ;
    eosio_assert( eoscrazytown::checkBets( eos, memo, vbets, totalBets ), "Bets not equal to amount.");
    eosio_assert( totalBets >= 1, "Bets should >= 1=");
    eosio_assert( totalBets <= 100, "Bets should not > 100");

    const auto& sym = eosio::symbol_type(EOS_SYMBOL).name();
    accounts eos_account(N(eosio.token), _self);
    auto old_balance = eos_account.get(sym).balance;

    eosio_assert( totalBets <= old_balance.amount / 2, "Bets too big");
    auto itr = players.find(from);
    if (itr == players.end()) {
        players.emplace(_self, [&](auto& p) {
            p.account = from;
            p.vbets = vbets ;
        });
    } else {
        eosio_assert( false, "Already bet.");
        return ;
    }
}*/

// input
void eoscrazytown::onTransfer(account_name from, account_name to, extended_asset quantity, string memo)
{
    if (to != _self) return;
    require_auth(from);
    //    eosio_assert(memo != "" , "must have something in memo") ;
    //    eosio_assert(memo.size() >= 21  , "bets wrong...") ;

    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "must buy a positive amount");
    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");    
    
    if (params[0] == "stake") {        
        eosio_assert(quantity.contract == N(dacincubator), "must use true CTN to stake");
        eosio_assert(quantity.symbol == CTN_SYMBOL, "must use CTN to stake");
        stake(from, quantity.amount);
        auto g = _global.get();
        g.total_staked += quantity.amount;
        _global.set(g, _self);     
        return;
    }    

    if (params[0] == "make_profit") {
        eosio_assert(quantity.contract == N(eosio.token), "must use true EOS to make profit");
        eosio_assert(quantity.symbol == EOS_SYMBOL, "must use EOS to make profit");
        make_profit(quantity.amount);
        return;
    }

}

auto eoscrazytown::getResult(const card &a, const card &b)
{
    string result = "XXXXXXXXXXX";
    const char o = 'O';
    auto aS = a / 13;
    auto bS = b / 13;
    auto aP = a % 13;
    auto bP = b % 13;
    aP ++;
    bP ++;

    if (aP > bP)
        result[0] = o; // (1)
    else if (aP < bP)
        result[1] = o; // (2)
    else
        result[2] = o; // (3)

    if (aS == 1 || aS == 2)
        result[3] = o; // (4) red
    else
        result[5] = o; // (6)

    if (bS == 1 || bS == 2)
        result[4] = o; // (5) red
    else
        result[6] = o; // (7)

    if ((aP & 1) == 1)
        result[7] = o; // (8) odd
    else
        result[8] = o; // (9)

    if ((bP & 1) == 1)
        result[9] = o; // (10) odd
    else
        result[10] = o; // (11)

    return result;
}

const vector<int64_t> eoscrazytown::getBets(const string &s, const char &c)
{ // need protect
    vector<int64_t> vbets;
    auto vs = explode(s, c);
    for (auto n : vs)
    {
        vbets.push_back((int64_t)string_to_price(n));
    }

    // change format
    vector<int64_t> v(vbets);
    v[0] = vbets[1];  // (1)
    v[1] = vbets[6];  // (2)
    v[2] = vbets[0];  // (3)
    v[3] = vbets[4];  // (4)
    v[4] = vbets[9];  // (5)
    v[5] = vbets[5];  // (6)
    v[6] = vbets[10]; // (7)
    v[7] = vbets[2];  // (8)
    v[8] = vbets[3];  // (9)
    v[9] = vbets[7];  // (10)
    v[10] = vbets[8]; // (11)

    return v;
}

auto eoscrazytown::getBeton(const vector<int64_t> &v)
{
    string beton = "";
    for (auto n : v)
        beton += (n != 0) ? 'O' : 'n';

    return beton;
}

const int64_t eoscrazytown::getTotalBets(const vector<int64_t> &v)
{
    int64_t totalBets = 0;
    for (auto n : v)
        totalBets += n;

    return totalBets;
}

uint64_t merge_seed(const checksum256 &s1, const checksum256 &s2) {
    uint64_t hash = 0, x;
    for (int i = 0; i < 32; ++i) {
        hash ^= (s1.hash[i]) << ((i & 7) << 3);
      //  hash ^= (s1.hash[i] ^ s2.hash[31-i]) << ((i & 7) << 3);
    }
    return hash;
}

// Output
void eoscrazytown::reveal(const checksum256 &seed, const checksum256 &hash)
{ // hash for next round
    require_auth(_self);

    auto g = _global.get();

 //   assert_sha256((char *)&seed, sizeof(seed), (const checksum256 *)&g.hash);

    eosio_assert(! (g.hash == checksum256(hash)), "same hash");
    g.hash = hash;


 //   eosio_assert(players.begin() != players.end(), "must have at least one player");
    auto t = merge_seed(seed, seed);
    card dragon = t % 52;
    card tiger = t / 52 % 52;
    const rec_reveal _reveal{
        .dragon = dragon,
        .tiger = tiger,
        .server_hash = _global.get().hash,
        .client_seed = seed,
    };

    action( // give result to client
        permission_level{_self, N(active)},
        _self, N(receipt), _reveal)
        .send();

    auto result = getResult(_reveal.dragon, _reveal.tiger);

    string beton;
    string presult = "";
    int64_t bonus;
    const char o = 'O';
    for (const auto &p : players)
    {
        auto &bets = p.vbets;
        beton = getBeton(bets);

        bonus = 0;
        presult = "";
        // exp:
        // r 2.0: O X X X X X X X X X X // no space !
        // beton: O X X O X X O X O O O // no space !
        if (result[2] == o)
        { // (3) draw
            if (result[2] == beton[2])
                bonus += bets[2] + bets[2] * DRAW; // (3)
            bonus += bets[0] / 2;
            bonus += bets[1] / 2;
            /*
            if ( result[3] == beton[3] ) bonus += bets[3] + bets[3] * COLOR ; // (4)
            if ( result[4] == beton[4] ) bonus += bets[4] + bets[4] * COLOR ; // (5)

            if ( result[5] == beton[5] ) bonus += bets[5] + bets[5] * COLOR ; // (6)
            if ( result[6] == beton[6] ) bonus += bets[6] + bets[6] * COLOR ; // (7)
            */
        }
        else
        {
            if (result[0] == o)
            {
                if (result[0] == beton[0])
                {
                    bonus += bets[0] + bets[0] * SIDE; // (1)
                    presult += '1';
                }
            }
            else
            { // result[1] == o
                if (result[1] == beton[1])
                {
                    bonus += bets[1] + bets[1] * SIDE; // (2)
                    presult += '2';
                }
            }

            if (result[3] == beton[3])
            {
                bonus += bets[3] + bets[3] * COLOR; // (4)
                presult += '4';
            }

            if (result[4] == beton[4])
            {
                bonus += bets[4] + bets[4] * COLOR; // (5)
                presult += '5';
            }

            if (result[5] == beton[5])
            {
                bonus += bets[5] + bets[5] * COLOR; // (6)
                presult += '6';
            }
            if (result[6] == beton[6])
            {
                bonus += bets[6] + bets[6] * COLOR; // (7)
                presult += '7';
            }

            if (result[7] == beton[7])
            {
                bonus += bets[7] + bets[7] * ODD; // (8)
                presult += '8';
            }
            if (result[8] == beton[8])
            {
                bonus += bets[8] + bets[8] * EVEN; // (9)
                presult += '9';
            }
            if (result[9] == beton[9])
            {
                bonus += bets[9] + bets[9] * ODD; // (10)
                presult += 'A';
            }
            if (result[10] == beton[10])
            {
                bonus += bets[10] + bets[10] * EVEN; // (11)
                presult += 'B';
            }
        }

        if(bonus == 0) {         
            auto _amountOfEos = (getTotalBets(bets) - bonus) * 5 / 1000;
            auto _itr = _CNTmarket.begin();
            const auto& _tokenSupply = _itr->supply;
            const auto& _eosSupply = _itr->balance;

            auto _eosTotalAmount = _eosSupply.amount + _amountOfEos;
            // with no fee
            uint64_t new_supply = sqrt((real_type)_eosTotalAmount * 2 * K) * 100;
            uint64_t delta_supply = new_supply - _tokenSupply.amount;

            send_defer_action(
                permission_level{_self, N(active)},
                N(dacincubator), N(transfer),
                make_tuple(_self, p.account, asset(delta_supply, CTN_SYMBOL),
                        string("Better next time")));

            _CNTmarket.modify(_itr, 0, [&](auto &t) {
                t.supply.amount = new_supply;
                t.balance.amount = ((real_type)new_supply * new_supply) / 2 / K / 10000;
            });            
        }
        else if(bonus<=2000000){            
            send_defer_action(
                permission_level{_self, N(active)},
                N(eosio.token), N(transfer),
                make_tuple(_self, p.account, asset(bonus, EOS_SYMBOL),
                        string("Winner Winner Chicken Dinner. " + presult)));                
        } 
    }

   
    g.dragon = dragon;
    g.tiger = tiger;
    _global.set(g, _self);

    while (players.begin() != players.end())
    {
        players.erase(players.begin());
    }

    // init( hash ) ;
}


