class FNV
{
public:
   FNV() = default;
   virtual ~FNV() {}
   virtual size_t operator()( const char *data ) const
      {
      //TODO optimize 
      size_t length = strlen(data);

      static const size_t FnvOffsetBasis=146959810393466560;
      static const size_t FnvPrime=1099511628211ul;
      size_t hash = FnvOffsetBasis;
      for (  size_t i = 0; i < length; ++i)
         {
         hash *= FnvPrime;
         hash ^= (unsigned long)data[ i ];
         } // end for
      return hash;
      } //end operator()
};