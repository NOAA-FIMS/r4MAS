/* 
 * File:   Inverse3.hpp
 * Author: matthewsupernaw
 *
 * Created on July 1, 2015, 2:03 PM
 */

#ifndef GJE_HPP
#define	GJE_HPP
#include "SIMD.hpp"

namespace gje {

    template<class T>
    struct matrix {
        /*__declspec(align(16))*/ T* data;

        size_t isize;
        size_t jsize;

        matrix(int isize, int jsize) : isize(isize), jsize(jsize), data(new T[isize*jsize]) {

        }

        virtual ~matrix() {
            delete[] data;
        }

        void make_identity() {
            for (int i = 0; i < isize; i++) {
                for (int j = 0; j < isize; j++) {
                    if (i == j) {
                        data[i * jsize + j] = 1.0;
                    } else {
                        data[i * jsize + j] = 0.0;
                    }
                }
            }
        }

        inline T& operator()(const int& i, const int& j) const {
            return data[i * jsize + j];
        }

        inline T& operator()(const int& i, const int& j) {
            return data[i * jsize + j];
        }


    };

    template<class T>
    std::ostream& operator<<(std::ostream& out, matrix<T>& m) {
        for (int i = 0; i < m.isize; i++) {
            for (int j = 0; j < m.jsize; j++) {
                out << m(i, j) << " ";
            }
            out << "\n";
        }
        return out;
    }

    class ThreadPool {
    public:

        ThreadPool(int threads = std::thread::hardware_concurrency()) : shutdown_(false) {
            // Create the specified number of threads
            threads_.reserve(threads);
            for (int i = 0; i < threads; ++i)
                threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
        }

        ~ThreadPool() {
            {
                // Unblock any threads and tell them to stop
                std::unique_lock <std::mutex> l(lock_);

                shutdown_ = true;
                condVar_.notify_all();
            }

            // Wait for all threads to stop
            for (auto& thread : threads_)
                thread.join();
        }

        size_t Size() {
            return threads_.size();
        }

        void doJob(std::function <void (void) > func) {
            // Place a job on the queue and unblock a thread
            std::unique_lock <std::mutex> l(lock_);

            jobs_.emplace(std::move(func));
            condVar_.notify_one();
        }

        void wait() {
            while (1) {
                lock_.lock();
                bool working = jobs_.size();
                lock_.unlock();
                if (!working) {
                    break;
                }
            }
        }



    protected:

        void threadEntry(int i) {
            std::function <void (void) > job;

            while (1) {
                {
                    std::unique_lock <std::mutex> l(lock_);

                    while (!shutdown_ && jobs_.empty())
                        condVar_.wait(l);

                    if (jobs_.empty()) {
                        // No jobs to do and we are shutting down
                        return;
                    }

                    job = std::move(jobs_.front());
                    jobs_.pop();
                }

                // Do the job without holding any locks
                job();
            }

        }


        std::mutex lock_;
        std::condition_variable condVar_;
        bool shutdown_;
        std::queue <std::function <void (void) >> jobs_;
        std::vector <std::thread> threads_;
    };

    template<class T>
    class GJEWorker {
        matrix<T>& A_m;
        matrix<T>& B_m;
        int cstart;
        int cend;
        int rstart;
        int rend;
        int dindex;
        T tempval;
#ifdef USE_SSE
        typedef typename simd::simd_traits<T>::type type;
#endif

    public:

        //        GEWorker(const GEWorker<T>& other) {
        //            A_m = other.A_m;
        //            B_m = other.B_m;
        //            workers = other.workers;
        //        }

        GJEWorker(matrix<T>& A, matrix<T>& B) :
        A_m(A),
        B_m(B) {
        }

        inline void PreparePhase1(int dindex, int cstart, int cend, T tempval) {
            this->dindex = dindex;
            this->cstart = cstart;
            this->cend = cend;
            this->tempval = tempval;

        }

        inline void PreparePhase2(int dindex, int rstart, int rend, int cstart, int cend) {
            this->dindex = dindex;
            this->rstart = rstart;
            this->rend = rend;
            this->cstart = cstart;
            this->cend = cend;

        }

        inline void PreparePhase3(int dindex, int rstart, int rend, int cstart, int cend) {
            this->dindex = dindex;
            this->rstart = rstart;
            this->rend = rend;
            this->cstart = cstart;
            this->cend = cend;

        }

        void Phase1() {
#ifdef USE_SSE
            int col;
            type a;
            type b;
            type temp(tempval);
            int stop = cend / simd::simd_traits<T>::size;
            for (col = cstart; col < stop; col += simd::simd_traits<T>::size) {
                a.load_u(&A_m(dindex, col));
                b.load_u(&B_m(dindex, col));
                a *= temp;
                b *= temp;
                a.store_u(&A_m(dindex, col));
                b.store_u(&B_m(dindex, col));
            }
            for (; col < cend; col++) {
                A_m(dindex, col) *= tempval;
                B_m(dindex, col) *= tempval;
            }
#else
            int col;
            for (col = cstart; col < cend; col++) {
                A_m(dindex, col) *= tempval;
                B_m(dindex, col) *= tempval;
            }
#endif
        }

        void Phase2() {
#ifdef USE_SSE

            int row;
            int col;
            type a;
            type b;
            type a2;
            type b2;
            int stop = cend / simd::simd_traits<T>::size;
            for (row = rstart; row < rend; ++row) {
                T wval2 = A_m(row, dindex);
                type wval(wval2);
                for (col = cstart; col < stop; col += simd::simd_traits<T>::size) {
                    a.load_u(&A_m(row, col));
                    b.load_u(&B_m(row, col));
                    a2.load_u(&A_m(dindex, col));
                    b2.load_u(&B_m(dindex, col));
                    a -= wval2*a2;
                    b -= wval2*b2;
                    a.store_u(&A_m(row, col));
                    b.store_u(&B_m(row, col));
                }
                for (;col < cend; col = col + 1) {
                    A_m(row, col) -= wval2 * A_m(dindex, col);
                    B_m(row, col) -= wval2 * B_m(dindex, col);
                }
            }

#else
            int row;
            int col;
            for (row = rstart; row < rend; ++row) {
                T wval = A_m(row, dindex);
                for (col = cstart; col < cend; col = col + 1) {
                    A_m(row, col) -= wval * A_m(dindex, col);
                    B_m(row, col) -= wval * B_m(dindex, col);
                }
            }
#endif
        }

        void Phase3() {
#ifdef USE_SSE
            int row;
            int col;
            type a;
            type b;
            type a2;
            type b2;
            int stop = cend / simd::simd_traits<T>::size;
            
            for (row = rstart; row >= rend; --row) {
                T wval2 = A_m(row, dindex);
                type wval(wval2);
                for (col = cstart; col < stop; col += simd::simd_traits<T>::size) {
                    a.load_u(&A_m(row, col));
                    b.load_u(&B_m(row, col));
                    a2.load_u(&A_m(dindex, col));
                    b2.load_u(&B_m(dindex, col));
                    a -= wval*a2;
                    b -= wval*b2;
                    a.store_u(&A_m(row, col));
                    b.store_u(&B_m(row, col));
                }

                for (;col < cend; col = col + 1) {
                    A_m(row, col) -= wval2 * A_m(dindex, col);
                    B_m(row, col) -= wval2 * B_m(dindex, col);
                }
            }
#else
            int row;
            int col;
            for (row = rstart; row >= rend; --row) {
                T wval = A_m(row, dindex);

                for (col = cstart; col < cend; col = col + 1) {
                    A_m(row, col) -= wval * A_m(dindex, col);
                    B_m(row, col) -= wval * B_m(dindex, col);
                }
            }
#endif
        }
    };

    template<class T>
    void LaunchGEWorker1(GJEWorker<T>& worker, int phase) {
        worker.Phase1();
    }

    template<class T>
    void LaunchGEWorker2(GJEWorker<T>& worker, int phase) {
        worker.Phase2();
    }

    template<class T>
    void LaunchGEWorker3(GJEWorker<T>& worker, int phase) {
        worker.Phase3();
    }

    template<class T>
    inline void swaprows(matrix<T>& m, size_t row0, size_t row1, T* temp) {

        for (int i = 0; i < m.jsize; i++) {
            temp[i] = m(row0, i);
            m(row0, i) = m(row1, i);
            m(row1, i) = temp[i];
        }
    }

    template<class T>
    void Inverse(matrix<T>& A, matrix<T>& B, size_t nrows, bool concurrent) {
        size_t nthreads = std::thread::hardware_concurrency();
        size_t crange = nrows / nthreads;
        T* temp = new T[A.jsize]; //work space for swapping

        //thread pool
        ThreadPool tpool(nthreads);

        //workers for thread pool
        std::vector<GJEWorker<T> > workers;
        for (int i = 0; i < nthreads; i++) {
            workers.push_back(GJEWorker<T>(A, B));
        }

        /**
         * Gaussian elimination start
         */
        for (size_t dindex = 0; dindex < nrows; ++dindex) {

            if (A(dindex, dindex) == 0) {
                swaprows(A, dindex, dindex + 1, temp);
                swaprows(B, dindex, dindex + 1, temp);
            }


            T tempval = 1.0 / A(dindex, dindex);


            if (concurrent) {

                for (int t = 0; t < nthreads; t++) {
                    int cstart = (t * crange);
                    int cend = (t + 1) * crange;

                    if (t == (nthreads - 1)) {
                        cend = nrows;
                    }
                    workers[t].PreparePhase1(dindex, cstart, cend, tempval);
                    tpool.doJob(std::bind(LaunchGEWorker1<T>, std::ref(workers[t]), 1));
                }
                tpool.wait();

                size_t rrange = (nrows - (dindex + 1)) / nthreads;

                if (rrange > nthreads) {

                    for (int t = 0; t < nthreads; t++) {

                        int rstart = (dindex + 1)+(t * rrange);
                        int rend = (dindex + 1)+ (t + 1) * rrange;

                        if (t == (nthreads - 1)) {
                            rend = nrows;
                        }

                        workers[t].PreparePhase2(dindex, rstart, rend, 0, nrows);
                        tpool.doJob(std::bind(LaunchGEWorker2<T>, std::ref(workers[t]), 2));
                    }
                    tpool.wait();
                } else {

                    for (size_t row = (dindex + 1); row < nrows; ++row) {
                        T wval = A(row, dindex);
                        for (size_t col = 0; col < nrows; col = col + 1) {
                            A(row, col) -= wval * A(dindex, col);
                            B(row, col) -= wval * B(dindex, col);
                        }
                    }

                }
            
            } else {

                for (size_t col = 0; col < nrows; col++) {
                    A(dindex, col) *= tempval;
                    B(dindex, col) *= tempval;
                }

                for (size_t row = (dindex + 1); row < nrows; ++row) {
                    T wval = A(row, dindex);
                    for (size_t col = 0; col < nrows; col = col + 1) {
                        A(row, col) -= wval * A(dindex, col);
                        B(row, col) -= wval * B(dindex, col);
                    }
                }
                
            }
        }

    
        /**
         *Back substitution
         */

        if (concurrent) {


            for (long dindex = nrows - 1; dindex >= 0; --dindex) {
                int rrange = ((dindex - 1)) / nthreads;

                if (dindex > nthreads) {
                    for (int t = 0; t < nthreads; t++) {
                        int rstart = (dindex - 1) - (t) * rrange;
                        int rend = (dindex - 1) - (t + 1) * rrange;
                        
                        if (t == (nthreads - 1)) {
                            rend = 0;
                        }

                        workers[t].PreparePhase3(dindex, rstart, rend, 0, nrows);
                        tpool.doJob(std::bind(LaunchGEWorker3<T>, std::ref(workers[t]), 3));
                    }
                    tpool.wait();

                } else {

                    for (long row = dindex - 1; row >= 0; --row) {
                        T wval = A(row, dindex);
                        for (size_t col = 0; col < nrows; col = col + 1) {
                            A(row, col) -= wval * A(dindex, col);
                            B(row, col) -= wval * B(dindex, col);
                        }
                    }
                }
            }
        } else {

            for (long dindex = nrows - 1; dindex >= 0; --dindex) {
                for (long row = dindex - 1; row >= 0; --row) {
                    T wval = A(row, dindex);
                    for (size_t col = 0; col < nrows; col = col + 1) {
                        A(row, col) -= wval * A(dindex, col);
                        B(row, col) -= wval * B(dindex, col);
                    }
                }
            }
        }
        delete[] temp;
    }

    void Test2() {
        std::ifstream in;
        in.open("data.txt");
        int N = 1500;
        in>>N;
        //        N = 10;

        typedef double real;

        matrix<real> A(N, N);
        matrix<real> B(N, N);
        matrix<real> A2(N, N);
        matrix<real> B2(N, N);
        B.make_identity();
        B2.make_identity();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A(i, j) = rand() + .01; // = 0.0;
                A2(i, j) = A(i, j);
            }
        }

        //                A(0, 0) = 5.0;
        //                A(0, 1) = -3.0;
        //                A(0, 2) = 2.0;
        //        
        //                A(1, 0) = -3.0;
        //                A(1, 1) = 2.0;
        //                A(1, 2) = -1.0;
        //        
        //                A(2, 0) = -3.0;
        //                A(2, 1) = 2.0;
        //                A(2, 2) = -2.0;



        std::cout << "concurrent run..." << std::flush;
        auto eval_start = std::chrono::steady_clock::now();
        Inverse(A, B, N, true);
        auto eval_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> eval_time = eval_end - eval_start;
        std::cout << "sequential run..." << std::flush;
        auto seval_start = std::chrono::steady_clock::now();
        Inverse(A2, B2, N, false);
        auto seval_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> seval_time = seval_end - seval_start;
        std::cout << "done!\n";

        if (N <= 20) {
            std::cout << B << "\n\n\n" << B2 << "\n\n";
        }


        std::cout << N << " x " << N << "\n";
        std::cout << "concurrent time = " << (eval_time.count()) << " sec\n";
        std::cout << "sequential time = " << (seval_time.count()) << " sec\n";
        std::cout << "speed up = " << (seval_time.count() / eval_time.count()) << " sec\n";

    }


}



#endif	/* GJE_HPP */

