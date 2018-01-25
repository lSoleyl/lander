#pragma once

/** This class wraps a releasable interface pointer, which gets released if the resource
 *  object falls out of scope. The resource object can be used in the same way as the interface
 *  pointer itself with the exception that Release() shouldn't be called on it.
 */
template<typename Interface>
class Resource {
public:
  Resource() : ptr(nullptr) {}
  Resource(Interface* iPtr) : ptr(iPtr) {}
  Resource(Resource<Interface>&& other) : ptr(nullptr) {
    std::swap(ptr, other.ptr);
  }

  /** Move assign a new interface pointer. Any currently held interface pointer
   *  gets released before this operation.
   */
  Resource<Interface>& operator=(Resource<Interface>&& other) {
    if (ptr != other.ptr) { //protect against self assignment
      reset();
      std::swap(ptr, other.ptr);
    }
    return *this;
  }

  /** Change the internal pointer to the passed pointer
   */
  Resource<Interface>& operator=(Interface* iPtr) {
    reset(iPtr);
    return *this;
  }
  
  /** Resource gets released in the object's destructor
   */
  ~Resource() {
    reset();
  }

  /** Release the internally held pointer and replace it with the given pointer.
   *
   * @param iPtr the new interface pointer of this resource object
   */
  void reset(Interface* iPtr = nullptr) {
    if (ptr && ptr != iPtr) {
        ptr->Release();
    }    

    ptr = iPtr;
  }

  template<typename TargetInterface>
  TargetInterface as() { return dynamic_cast<TargetInterface>(ptr); }

  /** Implicit cast to the internally held interface pointer
   */
  operator Interface*() const {
    return ptr;
  }

  /** Implicit bool cast to check for nullptr
   */
  operator bool() const {
    return ptr != nullptr;
  }

  Interface* operator->() const {
    return ptr;
  }

  /** Releases any previously held interface pointer and returns the address of the internal pointer.
   *  The currently held pointer is released to safely overwrite the pointer with a new interface pointer.
   */
  Interface** operator&() {
    reset(); 
    return &ptr;
  }




private:
  Interface* ptr; //internal pointer
};