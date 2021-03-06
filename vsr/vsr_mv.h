#ifndef MV_H_INCLUDED
#define MV_H_INCLUDED  

#include <math.h>   
#include "vsr_basis.h"       

namespace vsr{

#define CA constexpr auto
#define SCA static constexpr auto  
#define SC static constexpr  
#define RETURNS(X) ->decltype(X){return X;}

//FUNCTION HELPER
template<bool>
struct May{
  template<class A, class B>
  static constexpr A Be(const A& a, const B& b) { return a; } 
};
template<>
struct May<false>{
  template<class A, class B>
  static constexpr B Be(const A& a, const B& b) { return b; }
};  

template<bool, class A, class B>
struct Maybe{
  typedef A Type;  
};
template<class A, class B>
struct Maybe<false,A,B>{
  typedef B Type;  
}; 

struct Space{
  static void print() { printf("\n");}  
};
  

template<TT ... XS>
struct MV{ 
  static const int Num = 0;
  static void print(){ printf("\n");} 
  static void bprint(){ printf("\n");} 
  static void vprint(){ printf("nil\n");}  
};   


template<TT X, TT ... XS>
struct MV<X, XS...>{  
  
  static const int Num = sizeof...(XS) +1;  
  VT val[Num];
  
  typedef const VT array_type[Num]; 
  array_type& begin() const { return val; } 
  
  static const TT HEAD = X;
  typedef MV<XS...> TAIL;  
  
  
  template<typename...Args>     
  constexpr explicit MV(Args...v) : val{ static_cast<VT>(v)...} {}    
    
  template<class A> A cast() const;   
  template<class A> A copy() const; 
  
  // template<TT N> 
  // A sub() const; 
  
  template<typename A>
  MV& operator = ( const A& a) {
    *this = a.template cast< MV<X,XS...> >(); 
    return *this;
  }
  
  template<TT IDX> VT get() const;
  template<TT IDX> VT& get(); 
  
  template<TT IDX> MV& set(VT v);
  
  MV& reset(VT v = 0.0){
    std::fill( &(val[0]), &(val[0]) + Num, v); 
    return *this;
  }
  
  MV conjugation() const;
  MV involution() const; 
   
  constexpr VT operator[] (int idx) const{
    return val[idx]; 
  }
  VT& operator[] (int idx) {
    return val[idx]; 
  }  

    bool operator == (const MV& mv) const{
    for (int i = 0; i < Num; ++i) {
      if (val[i] != mv[i]) return false;
    }
        return true;
    }  
  
  void print() { printf("blades\n"); bprint(); vprint(); } 
  static void bprint() { printf("%s\t%s\t%d\n", estring(X).c_str(), bitString<6>(X).c_str(), X);  TAIL::bprint(); } 
  void vprint() const { for ( float i : val ) printf("%f\t", i); printf("\n\n"); }   
        

   
}; 
 

template<TT ... XS, TT ... YS>
constexpr MV<XS...,YS...> cat ( const MV<XS...>&, const MV<YS...>&){  
  return     MV<XS...,YS...>() ;
}  

template<class A, class B>
struct Cat{   
  typedef MV<> Type;
};

template<TT ... XS, TT ... YS>
struct Cat< MV<XS...>, MV<YS...> > {
  typedef  MV<XS..., YS...> Type; 
}; 

//sort as you add in . . .
/* template <int A, class Rest, class First = MV<> > */
/* struct Insert{ */        
/*   typedef typename Maybe< compare<A, Rest::HEAD>(), */
/*     typename Cat< typename Cat< First, MV<A> >::Type, Rest >::Type, */
/*     typename Insert< A, typename Rest::TAIL, typename Cat< First, MV<Rest::HEAD> >::Type>::Type */
/*   >::Type Type; */ 
/* }; */
/* template <int A, class First> */
/* struct Insert<A, MV<>, First>{ */
/*   typedef typename Cat< First, MV<A> >::Type Type; */
/* }; */



/*-----------------------------------------------------------------------------
 *  TAKE FIRST S elements
 *-----------------------------------------------------------------------------*/
template<int S,class B>
struct Take{
  using Type = typename Cat< MV<B::HEAD>, typename Take<S-1,typename B::TAIL>::Type>::Type;
};

template<class B>
struct Take<0,B>{
  using Type = MV<>;
};


/*-----------------------------------------------------------------------------
 *  REMOVE FIRST S elements
 *-----------------------------------------------------------------------------*/
template<int S, class B>
struct Remove{
  using Type = typename Remove<S-1, typename B::TAIL>::Type;
};
template<class B>
struct Remove<0,B>{
  using Type = B;
};


/*-----------------------------------------------------------------------------
 *  INSERT SORT
 *-----------------------------------------------------------------------------*/
template<bool B>
struct InsertIdxOfExists;

template<bool B>
struct InsertIdxOfImpl;

template<bool B>
struct InsertIdxOfEnd;


template<>
struct InsertIdxOfExists<true>{
  template<int A, class B>
  static constexpr int Call(int c) { return -1; }
};

template<>
struct InsertIdxOfImpl<true>{
  template<int A, class B>
  static constexpr int Call(int c) { return c==0 ? 0 : c-1; }
};

template<>
struct InsertIdxOfEnd<true>{
  template<int A, class B>
  static constexpr int Call(int c) { return A==B::HEAD ? -1 : compare(A,B::HEAD) ? c : c+1; }
};


template<>
struct InsertIdxOfImpl<false>{
  template<int A, class B>
  static constexpr int Call(int c) { 
    return InsertIdxOfEnd< B::TAIL::Num == 0 >::            // Is Last Element?
      template Call<A, B>(c); 
  }
};


template<>
struct InsertIdxOfEnd<false>{
  template<int A, class B>
  static constexpr int Call(int c) { 
    return InsertIdxOfExists< A == B::HEAD >::              // Already Exists?
      template Call<A, B>(c);
  }    
};

template<>
struct InsertIdxOfExists<false>{
  template<int A, class B>
  static constexpr int Call(int c) { 
    return InsertIdxOfImpl< compare(A, B::HEAD) >:: // Is in Correct Position?
      template Call<A, typename B::TAIL>(c+1); 
  }
};

template<int A, class B>
struct InsertIdxOf{
  static constexpr int Call(){
      return InsertIdxOfImpl< compare(A, B::HEAD) >::
        template Call<A, B>(0); 
  }
};

template<int A>
struct InsertIdxOf<A, MV<>> {
  static constexpr int Call(int c = 0){
      return c;
  }
};

template<bool b>
struct InsertImpl;

template<>
struct InsertImpl<false>{
  template<int A, int S, class B>
  using Result = 
    typename Cat< 
      typename Take<S, B>::Type,
      typename Cat<MV<A>, typename Remove<S,B>::Type>::Type
    >::Type;
};

template<>
struct InsertImpl<true>{
  template<int A, int S, class B>
  using Result = B;
};


template<int A, class B>
struct Insert{
  static const int Switch = InsertIdxOf<A,B>::Call();
  using Type = typename InsertImpl< Switch == -1 >::template Result<A,Switch,B>;
};





//cat insert A into B
template<class A, class B>
struct ICat{   
  typedef typename Insert< A::HEAD, B>::Type One;                        
  typedef typename ICat < typename A::TAIL, One  >::Type Type;
};
template<class B>
struct ICat< MV<>, B>{
  typedef B Type;
};


//Return Sub B not in A
template<class A, class B>
struct NotType{  
  static const int IS = find( B::HEAD, A() );
  typedef typename Cat< 
    typename Maybe< IS == -1,
      MV< B::HEAD > , 
      MV<>
    >::Type,
      typename NotType< A, typename B::TAIL >::Type 
  >::Type Type;  
};  
template<class A>
struct NotType< A, MV<> >{
  typedef MV<> Type;  
};

template<class M, int AB, int SF>  
struct MSign{ 
  static constexpr int Val =  (AB & 1) ? MSign< typename M::TAIL, (AB>>1) , ( SF * ( M::HEAD ) ) >::Val : MSign<typename M::TAIL, (AB>>1), SF >::Val; 
};  
template<class M, int SF >
struct MSign<M, 0, SF>{
  static constexpr int Val = SF;
};
template<int AB, int SF >
struct MSign<MV<>, AB, SF >{
  static constexpr int Val = SF;
};
template<int SF >
struct MSign<MV<>, 0, SF >{
  static constexpr int Val = SF;
};

template<int N>
constexpr TT bit(){ return 1 << N; }

template<TT TOT, TT DIM = TOT>
struct Blade1{  
  static constexpr auto Vec() RETURNS(
    cat( MV< bit<(TOT - DIM)>() >(), Blade1< TOT, DIM-1>::Vec() )
  ) 
  typedef decltype( Vec() ) VEC;           
};
template<TT TOT>
struct Blade1<TOT,0>{  
  static constexpr auto Vec() RETURNS(
    MV<>()
  )
};

 
template<int P>
struct RPlus{
  typedef typename Cat< MV<1>, typename RPlus<P-1>::Type>::Type Type;  
};
template<>
struct RPlus<0>{
  typedef MV<> Type;
};

template<int P>
struct RMinus{
  typedef typename Cat< MV<-1>, typename RMinus<P-1>::Type>::Type Type;  
};
template<>
struct RMinus<0>{
  typedef MV<> Type;
};

template<int P, int Q>   
struct RMetric{
  typedef typename Cat< typename RPlus<P>::Type, typename RMinus<Q>::Type >::Type Type;  
};


template<TT N, class M>
struct Exists{  
  static constexpr bool Call() { return M::HEAD == N ? true : Exists<N, typename M::TAIL>::Call(); }  
};

template<TT N>
struct Exists< N, MV<> >{  
  static constexpr bool Call() { return false; }
};

/* template<class T> */
/* struct Grade { */
/*    typedef Grade< typename T::TAIL > Tail; */
/*    SCA Call() RETURNS ( */ 
/*     MV< grade( T::HEAD ) > */
/* }; */

/* template<> */
/* struct Grade < MV<> >{ */
/*    SCA Call() RETURNS ( MV<>() ) */
/* } */


template<TT...XS> MV<XS...> 
sum( const MV<XS...> & a, const MV<XS...>& b) {
  MV<XS...> c;
  for (int i = 0; i < MV<XS...>::Num; ++i) c[i] = a[i] + b[i];
  return c;
} 
template<TT...XS> MV<XS...> 
diff( const MV<XS...> & a, const MV<XS...>& b) {
  MV<XS...> c;
  for (int i = 0; i < MV<XS...>::Num; ++i) c[i] = a[i] - b[i];
  return c;
}
template<TT...XS, TT...YS> 
typename ICat< typename NotType< MV<XS...>, MV<YS...> >::Type, MV<XS...> >::Type 
sum( const MV<XS...> & a, const MV<YS...>& b) {
  typedef typename ICat< typename NotType< MV<XS...>, MV<YS...> >::Type, MV<XS...> >::Type Ret; 
  return sum( a.template cast<Ret>() ,  b.template cast<Ret>() );
} 
template<TT...XS, TT...YS> 
typename ICat< typename NotType< MV<XS...>, MV<YS...> >::Type, MV<XS...> >::Type 
diff( const MV<XS...> & a, const MV<YS...>& b) {
  typedef typename ICat< typename NotType< MV<XS...>, MV<YS...> >::Type, MV<XS...> >::Type Ret;
  return diff( a.template cast<Ret>() ,  b.template cast<Ret>() );
}

template<TT ... XS>
typename ICat< typename NotType< MV<0>, MV<XS...> >::Type, MV<0> >::Type 
sumv( VT a, const  MV<XS...>& b) {
  typedef typename ICat< typename NotType< MV<0>,  MV<XS...> >::Type, MV<0> >::Type Ret;
  return sum( Ret(a) , b.template cast<Ret>() );
}

         

  

} //vsr::   

#endif
