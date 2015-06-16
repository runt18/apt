// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
// $Id: pkgrecords.h,v 1.6 2001/03/13 06:51:46 jgg Exp $
/* ######################################################################
   
   Package Records - Allows access to complete package description records
                     directly from the file.
   
   The package record system abstracts the actual parsing of the 
   package files. This is different than the generators parser in that
   it is used to access information not generate information. No 
   information touched by the generator should be parable from here as
   it can always be retreived directly from the cache.
   
   ##################################################################### */
									/*}}}*/
#ifndef PKGLIB_PKGRECORDS_H
#define PKGLIB_PKGRECORDS_H

#include <apt-pkg/pkgcache.h>
#include <apt-pkg/hashes.h>
#include <apt-pkg/macros.h>

#include <string>
#include <vector>

class pkgRecords							/*{{{*/
{
   public:
   class Parser;
   
   private:
   /** \brief dpointer placeholder (for later in case we need it) */
   void *d;
   
   pkgCache &Cache;
   std::vector<Parser *>Files;

    public:
   // Lookup function
   Parser &Lookup(pkgCache::VerFileIterator const &Ver);
   Parser &Lookup(pkgCache::DescFileIterator const &Desc);

   // Construct destruct
   explicit pkgRecords(pkgCache &Cache);
   virtual ~pkgRecords();
};
									/*}}}*/
class pkgRecords::Parser						/*{{{*/
{
   protected:
   
   virtual bool Jump(pkgCache::VerFileIterator const &Ver) = 0;
   virtual bool Jump(pkgCache::DescFileIterator const &Desc) = 0;
   
   public:
   friend class pkgRecords;
   
   // These refer to the archive file for the Version
   virtual std::string FileName() {return std::string();};
   virtual std::string SourcePkg() {return std::string();};
   virtual std::string SourceVer() {return std::string();};

   /** return all known hashes in this record.
    *
    * For authentication proposes packages come with hashsums which
    * this method is supposed to parse and return so that clients can
    * choose the hash to be used.
    */
   virtual HashStringList Hashes() const { return HashStringList(); };
#if APT_PKG_ABI >= 413
   APT_DEPRECATED std::string MD5Hash() const { return GetHashFromHashes("MD5Sum"); };
   APT_DEPRECATED std::string SHA1Hash() const { return GetHashFromHashes("SHA1"); };
   APT_DEPRECATED std::string SHA256Hash() const { return GetHashFromHashes("SHA256"); };
   APT_DEPRECATED std::string SHA512Hash() const { return GetHashFromHashes("SHA512"); };
#else
   APT_DEPRECATED std::string MD5Hash() { return GetHashFromHashes("MD5Sum"); };
   APT_DEPRECATED std::string SHA1Hash() { return GetHashFromHashes("SHA1"); };
   APT_DEPRECATED std::string SHA256Hash() { return GetHashFromHashes("SHA256"); };
   APT_DEPRECATED std::string SHA512Hash() { return GetHashFromHashes("SHA512"); };
#endif

   // These are some general stats about the package
   virtual std::string Maintainer() {return std::string();};
   /** return short description in language from record.
    *
    * @see #LongDesc
    */
   virtual std::string ShortDesc(std::string const &/*lang*/) {return std::string();};
   /** return long description in language from record.
    *
    * If \b lang is empty the "best" available language will be
    * returned as determined by the APT::Languages configuration.
    * If a (requested) language can't be found in this record an empty
    * string will be returned.
    */
   virtual std::string LongDesc(std::string const &/*lang*/) {return std::string();};
   std::string ShortDesc() {return ShortDesc("");};
   std::string LongDesc() {return LongDesc("");};

   virtual std::string Name() {return std::string();};
   virtual std::string Homepage() {return std::string();}

   // An arbitrary custom field
   virtual std::string RecordField(const char * /*fieldName*/) { return std::string();};

   // The record in binary form
   virtual void GetRec(const char *&Start,const char *&Stop) {Start = Stop = 0;};

   Parser();
   virtual ~Parser();

   private:
   void *d;
   APT_HIDDEN std::string GetHashFromHashes(char const * const type) const
   {
      HashStringList const hashes = Hashes();
      HashString const * const hs = hashes.find(type);
      return hs != NULL ? hs->HashValue() : "";
   };
};
									/*}}}*/
#endif
