/* Wrap a vector container and a mutex to be inserted in the unordered_map used by the eventbus */
#include<vector>
#include<memory>
#include<mutex>

template <typename T>
class vectorSync  {

	public:
		vectorSync() {
			mutexVector = new std::mutex();
		}

		/* Returns the vector */
		std::vector<T> &getVector()  {return vector;}
		/* Returns the mutex of the vector */
		std::mutex *getLock() {
			return mutexVector;
		}

	private:
		std::vector<T> vector;
		std::mutex *mutexVector;

};
