#include <queue>

#include <iostream>
// https://spg63.github.io/cppcommon/class_t_s_queue.html#details 
// Not my own code - but used it in the uDTC project when I was looking for a way 
// to push data efficiently out of DDR3 on our back-end processing board
// thread-safe queue to act as a buffer for the incoming data 
template <typename T>


class TSQueue {
private:
    // Underlying queue
    std::deque<T> m_queue;
  
    // mutex for thread synchronization
    std::mutex m_mutex;
  
    // Condition variable for signaling
    std::condition_variable m_cond;
    size_t m_loc{0};
  
public:
    void to_vector (std::vector<T>& pData, size_t pSize) { 
        std::unique_lock<std::mutex> lock(m_mutex);
        std::move(m_queue.begin(), m_queue.begin() + pSize , std::back_inserter(pData));
    }
    // erases N elements from the queue
    void erase(size_t pSize)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.erase( m_queue.begin(), m_queue.begin() + pSize ); 
    }

    // inserts vector into the queue 
    void insert(std::deque<T>& pInput, size_t pSize)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.insert( m_queue.end(), pInput.begin(), pInput.begin()+pSize);
    }
    // Pushes an element to the queue
    void push(T item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // Add item
        m_queue.push_back(item);
        m_loc++;

        // Notify one thread that
        // is waiting
        m_cond.notify_one();
        
    }
    // peeks at what is at the start of the queue 
    T front()
    {
        // acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // wait until queue is not empty
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });
        T item = m_queue.front();
        return item;      
    }
    // Pops an element off the queue
    T pop()
    {
        // acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // wait until queue is not empty
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });
        // T item = m_vector[m_loc];
        T item = m_queue.front();
        m_queue.pop_front();
        m_loc--; 
        return item;
    }
    bool empty()
    {
        // std::unique_lock<std::mutex> lock(m_mutex);
        return size() == 0 ;
    }
    size_t size()
    {
        // return m_loc;
        // acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};


// Event queue which is made up of 64 bit words

// simple function to shift data 
// uint64_t shiftCounter(uint32_t pTimer, uint8_t pCycle ){ return ( (uint64_t)1 << (32+pCycle) ) | pTimer; }
// simple struct to hold event data 
struct Event {
    uint32_t fTimestamp=0xFFFFFFFF;
    uint32_t fEnergy=0xBADBAD;
    uint8_t  fCycle=0;
    void decode(uint64_t pWrd){ 
        fTimestamp = pWrd&0xFFFFFFFF;
        fEnergy = (pWrd >> 32 ) & 0xFFFFFFFF; 
    }
    uint64_t encode()
    {
        return ((uint64_t)(fEnergy) << 32) | (uint64_t)fTimestamp;
    }
    bool operator<( const Event& pVal ) const { 
        // uint64_t cMyTimer = (uint64_t)1 << (32+fCycle) ) | fTimestamp;
        // uint64_t cTimer = (uint64_t)1 << (32+pVal.fCycle) ) | pVal.fTimestamp;
        // std::cout << fTimestamp << "\n";
        return fTimestamp < pVal.fTimestamp; 
    }
};
using EventQueue = std::queue<Event> ;
