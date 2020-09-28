#include <bits/stdc++.h>

using namespace std;
typedef pair<int, int> pii;

int get_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int get_rand(int a, int b) {
    int x = uniform_int_distribution<int>(a, b)(rng);
    return x;
}

class DPSolve {
    vector<pii> coords;
    int n;
    int source;
    vector<vector<int>> dp;
public:
    DPSolve(const vector<pii> &coords, int source) : coords(coords), source(source) {
        n = coords.size();
        dp.resize((1 << n), vector<int>(n, -1));
    }

    int solve(int mask, int last) {
        if (mask == (1 << n) - 1) {
            return get_distance(coords[source].first, coords[source].second, coords[last].first, coords[last].second);
        }
        int &ret = dp[mask][last];
        if (ret != -1)
            return ret;
        int ans = 1e9 + 7;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i))
                continue;
            ans = min(ans, get_distance(coords[i].first, coords[i].second, coords[last].first, coords[last].second) +
                           solve(mask | (1 << i), i));

        }

        return ret = ans;
    }

    int solve() {
        int mask = 0;
        mask |= (1 << source);
        return solve(mask, source);
    }
};

class Individual {
public:
    vector<pii> tours;
    pii source;
    int n;
    int fitness; //fitness is the amount of distance traveled

    Individual(const vector<pii> &tours, const pii &source) {
        this->tours = tours;
        this->source = source;
        n = tours.size();
        fitness = calculate_fitness();
    }

    bool operator<(Individual &x) {
        return fitness < x.fitness;
    }

    Individual mate(Individual x) {
        map<pii, int> cnt;
        for (pii i:tours) {
            cnt[i]++;
        }
        vector<pii> new_tour;
        int nibo = get_rand(0, n);
        for (int i = 0; i < nibo; i++) {
            int mutation = get_rand(0,100);
            if(mutation<=10)
            {
                int idx = get_rand(i, n-1);
                swap(tours[i],tours[idx]);
            }
            new_tour.push_back(tours[i]);
            cnt[tours[i]]--;
            assert(cnt[tours[i]] >= 0);
        }
        for (int i = 0; i < n; i++) {
            if (cnt[x.tours[i]]) {
                cnt[x.tours[i]]--;
                new_tour.push_back(x.tours[i]);
            }
        }
        assert(new_tour.size() == n);
        return Individual(new_tour, source);
    }

    int calculate_fitness() {
        int d = 0;
        for (int i = 0; i < n; i++) {
            pii now = tours[i];
            pii prev = source;
            if (i - 1 >= 0)
                prev = tours[i - 1];
            d += get_distance(now.first, now.second, prev.first, prev.second);
        }
        d += get_distance(tours[n - 1].first, tours[n - 1].second, source.first, source.second);
        return d;
    }
};

class Genetic_Algorithm {
    vector<pii> tours;
    int n;
    pii source;
public:
    Genetic_Algorithm(const vector<pii> &t, int src) {
        n = tours.size();
        this->tours = t;
        this->source = tours[src];
        tours.erase(tours.begin() + src);
    }

    int solve() {
        vector<Individual> population;
        const int POPULATION_SIZE = 100;
        int ans = 1e9;
        for (int i = 0; i < POPULATION_SIZE; i++) {
            population.emplace_back(tours, source);
            shuffle(tours.begin(), tours.end(), rng);
        }

        for (int generation = 1; generation <= 1000; generation++) {
            sort(population.begin(), population.end());
            ans = min(ans, population[0].fitness);
            vector<Individual> new_generation;

            int s = (10 * POPULATION_SIZE) / 100;
            //10% of the fittest population goes to the next generation
            for (int i = 0; i < s; i++)
                new_generation.push_back(population[i]);
            //For the rest 90% we select 2 parent from randomly from the first 50% and mate them
            s = POPULATION_SIZE - s;
            for (int i = 0; i < s; i++) {
                int r = get_rand(0, 49);
                Individual parent1 = population[r];
                r = get_rand(0, 49);
                Individual parent2 = population[r];
                Individual offspring = parent1.mate(parent2);
                new_generation.push_back(offspring);
            }
            population = new_generation;

        }
        ans = min(ans, population[0].fitness);
        return ans;

    }
};

int main() {
    int n;
    cin >> n;
    vector<pii> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i].first >> a[i].second;
    }
    int src;
    cin >> src;
    assert(src >= 0 && src < n);
    if (n <= 15) {
        DPSolve d(a, src);
        cout << "Using Dp : " << d.solve() << "\n";
    }
    Genetic_Algorithm g(a, src);
    cout << "Using Genetic Algorithm : " << g.solve() << "\n";

    return 0;
}
