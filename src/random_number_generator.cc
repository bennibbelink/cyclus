#include "random_number_generator.h"

#include <iostream>
#include <string>

#include "logger.h"
#include "sim_init.h"
#include "context.h"

namespace cyclus{

    Generator RandomNumberGenerator::gen_;

    void RandomNumberGenerator::Initialize(SimInfo si){

        gen_.seed(si.seed);

        CLOG(LEV_INFO1) << "Pseudo random number generator initialized with seed: " << si.seed;
    }

    std::uint32_t RandomNumberGenerator::random(){
        return gen_();
    }

    double RandomNumberGenerator::random_01(){
        boost::random::uniform_01<> dist;
        return dist(gen_);
    }
    
    int RandomNumberGenerator::random_uniform_int(int low, int high){
        boost::random::uniform_int_distribution<> dist(low, high);
        boost::random::variate_generator<Generator&, boost::random::uniform_int_distribution<> > rn(gen_, dist);
        return rn();
    }

    double RandomNumberGenerator::random_uniform_real(double low, double high){
        boost::random::uniform_real_distribution<> dist(low, high);
        boost::random::variate_generator<Generator&, boost::random::uniform_real_distribution<> > rn(gen_, dist);
        return rn();
    }

    double RandomNumberGenerator::random_normal_real(double mean, double std_dev, double low, double high){
        boost::random::normal_distribution<> dist(mean, std_dev);
        boost::random::variate_generator<Generator&, boost::random::normal_distribution<> > rn(gen_, dist);
        double val = rn();
        while (val < low || val > high){
            val = rn();
        }
        return val;
    }

    int RandomNumberGenerator::random_normal_int(double mean, double std_dev, int low, int high){
        boost::random::normal_distribution<> dist(mean, std_dev);
        boost::random::variate_generator<Generator&, boost::random::normal_distribution<> > rn(gen_, dist);
        double val = rn();
        while (val < low || val > high){
            val = rn();
        }
        int rounded_val = std::lrint(val);
        return rounded_val;
    }

    //
    // Distributions
    UniformDoubleDist::UniformDoubleDist(double min, double max) : dist(min, max) {}

    double UniformDoubleDist::sample() { 
        return dist(RandomNumberGenerator::gen_); 
    }

    double UniformDoubleDist::max() { 
        return dist.max(); 
    }

    NormalDoubleDist::NormalDoubleDist(double mean, double std_dev, double min, double max) : dist(mean, std_dev) {
        min_ = min;
        max_ = max;
    }

    double NormalDoubleDist::sample() {
        double val = dist(RandomNumberGenerator::gen_);
        while (val < min_ || val > max_){
            val = dist(RandomNumberGenerator::gen_);
        }
        return val;
    }

    double NormalDoubleDist::max() { 
        return dist.max();
    }

    UniformIntDist::UniformIntDist(int min, int max) : dist(min, max) { }

    int UniformIntDist::sample() {
        return dist(RandomNumberGenerator::gen_);
    }

    NormalIntDist::NormalIntDist(double mean, double std_dev, int min, int max) : dist(mean, std_dev) {
        min_ = std::min(min, 0);
        max_ = max;
    }

    int NormalIntDist::sample() {
        double val = dist(RandomNumberGenerator::gen_);
        while (val < min_ || val > max_){
            val = dist(RandomNumberGenerator::gen_);
        }
        return std::lrint(val);
    }
}