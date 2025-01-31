#pragma once
#ifndef _SHARPEN_LEVELTABLE_HPP
#define _SHARPEN_LEVELTABLE_HPP

#include <set>

#include "LevelTableOption.hpp"
#include "LevelTableScanner.hpp"

namespace sharpen
{
    class LevelTable:public sharpen::Noncopyable
    {
    public:
        using MemTable = sharpen::MemoryTable<sharpen::BinaryLogger,sharpen::MemoryTableComparator>;
    private:
        using Self = sharpen::LevelTable;
        using ComponentMap = std::map<sharpen::Uint64,sharpen::LevelComponent>;
        using ViewMap = std::map<sharpen::Uint64,sharpen::LevelView>;
        using Comparator = sharpen::Int32(*)(const sharpen::ByteBuffer&,const sharpen::ByteBuffer&);
        using FileGenerator = sharpen::FileChannelPtr(*)(const char*,sharpen::FileAccessModel,sharpen::FileOpenModel);    

        //manifest
        //component: l{binaryLevel}
        //view: v{binaryId}
        //max level: ml
        //current table id: ctid
        //current view id: cvid
        //current memory table id: cmid
        //prev table id: ptid
        //prev view id:pvid
        //prev memory table id:pmid
        static sharpen::ByteBuffer currentTableIdKey_;
        static sharpen::ByteBuffer currentViewIdKey_;
        static sharpen::ByteBuffer currentMemTableIdKey_;
        static sharpen::ByteBuffer maxLevelKey_;
        static sharpen::ByteBuffer prevTableIdKey_;
        static sharpen::ByteBuffer prevViewIdKey_;
        static sharpen::ByteBuffer prevMemTableIdKey_;
        static std::once_flag keyFlag_;

        //format
        //table: {tableName}_{id}.{tableExtName}
        //wal: {tableName}_{id}.{walExtName}
        //manifest: {tableName}_manifest.{walExtName}
        std::string tableName_;
        std::string tableExtName_;
        std::string walExtName_;
        //maps
        mutable ComponentMap componentMap_;
        mutable ViewMap viewMap_;
        //table cache
        mutable sharpen::SegmentedCircleCache<sharpen::SortedStringTable> tableCaches_;
        //file generator
        FileGenerator fileGenerator_;
        //memory table
        std::unique_ptr<MemTable> mem_;
        //manifest
        std::unique_ptr<MemTable> manifest_;
        //immutable memory tables
        std::vector<std::unique_ptr<MemTable>> imMems_;
        //locks
        mutable std::unique_ptr<sharpen::AsyncReadWriteLock> levelLock_;
        mutable std::unique_ptr<sharpen::AsyncReadWriteLock> viewLock_;
        mutable std::unique_ptr<sharpen::AsyncReadWriteLock> componentLock_;
        //comparator
        Comparator comp_;
        //config
        sharpen::Size maxViewOfComponent_;
        sharpen::Size maxTableOfComponent_;
        sharpen::Size blockCacheSize_;
        sharpen::Size filterBitsOfElement_;
        sharpen::Size maxSizeOfMem_;
        sharpen::Size maxSizeOfImMems_;
        sharpen::Size blockSize_;
        std::unique_ptr<std::atomic_size_t> usedMemory_;
        //engine
        sharpen::EventEngine *engine_;

        static sharpen::ByteBuffer GetViewKey(sharpen::Uint64 id);

        static sharpen::ByteBuffer GetComponetKey(sharpen::Uint64 level);

        static void InitManifestKeys();

        sharpen::Uint64 GetCurrentTableId() const noexcept;

        sharpen::Uint64 GetCurrentViewId() const noexcept;

        sharpen::Uint64 GetCurrentMemoryTableId() const noexcept;

        sharpen::Uint64 GetPrevTableId() const noexcept;

        sharpen::Uint64 GetPrevViewId() const noexcept;

        sharpen::Uint64 GetPrevMemoryTableId() const noexcept;

        void SetCurrentTableId(sharpen::Uint64 id);

        void SetCurrentViewId(sharpen::Uint64 id);

        void SetMaxLevel(sharpen::Uint64 level);

        void SetCurrentMemoryTableId(sharpen::Uint64 id);

        void SetPrevTableId(sharpen::Uint64 id);

        void SetPrevViewId(sharpen::Uint64 id);

        void SetPrevMemoryTableId(sharpen::Uint64 id);

        sharpen::FileChannelPtr OpenChannel(const char *name,sharpen::FileAccessModel accessModel,sharpen::FileOpenModel openModel) const;

        sharpen::LevelView &GetView(sharpen::Uint64 id);

        const sharpen::LevelView &GetView(sharpen::Uint64 id) const;

        void SaveView(sharpen::Uint64 id,const sharpen::LevelView &view);

        sharpen::LevelComponent &GetComponent(sharpen::Uint64 id);

        const sharpen::LevelComponent &GetComponent(sharpen::Uint64 id) const;

        void SaveComponent(sharpen::Uint64 id,const sharpen::LevelComponent &component);

        sharpen::Size GetTableCount(const sharpen::LevelComponent &component) const;

        std::string FormatTableName(sharpen::Uint64 tableId) const;

        std::string FormatMemoryTableName(sharpen::Uint64 tableId) const;

        std::string FormatManifestName() const;

        sharpen::SortedStringTable MergeTables(const sharpen::LevelComponent &component,sharpen::Uint64 newTableId,bool eraseDeleted,sharpen::Optional<sharpen::SortedStringTable> appendTable);

        void AddToComponent(sharpen::SortedStringTable table,sharpen::Uint64 tableId,sharpen::Uint64 componentId);

        sharpen::SortedStringTable LoadTable(sharpen::Uint64 id) const;

        void DeleteTableFromCache(sharpen::Uint64 id);

        void DeleteTable(sharpen::Uint64 id);

        std::shared_ptr<sharpen::SortedStringTable> LoadTableFromCache(sharpen::Uint64 id) const;

        std::shared_ptr<sharpen::SortedStringTable> LoadTableCache(sharpen::Uint64 id) const;

        std::unique_ptr<MemTable> MakeNewMemoryTable();

        void DummpImmutableTables();

        void InitImmutableTables();

        void InitMemoryTable();

        void InitManifest();

        void GcTables();

        void GcViews();
    public:

        LevelTable(sharpen::EventEngine &engine,const std::string &tableName,const std::string &tableExtName)
            :LevelTable(engine,tableName,tableExtName,sharpen::LevelTableOption{})
        {}

        LevelTable(sharpen::EventEngine &engine,const std::string &tableName,const std::string &tableExtName,const sharpen::LevelTableOption &opt)
            :LevelTable(engine,tableName,tableExtName,"wal",opt)
        {}

        LevelTable(sharpen::EventEngine &engine,const std::string &tableName,const std::string &tableExtName,const std::string &walExtName,const sharpen::LevelTableOption &opt);
    
        LevelTable(Self &&other) noexcept;
    
        Self &operator=(Self &&other) noexcept;
    
        ~LevelTable() noexcept = default;

        sharpen::Int32 CompareKeys(const sharpen::ByteBuffer &left,const sharpen::ByteBuffer &right) const noexcept;

        void Put(sharpen::ByteBuffer key,sharpen::ByteBuffer value);

        void Delete(sharpen::ByteBuffer key);

        void Action(sharpen::WriteBatch batch);

        sharpen::ExistStatus Exist(const sharpen::ByteBuffer &key) const;

        sharpen::Optional<sharpen::ByteBuffer> TryGet(const sharpen::ByteBuffer &key) const;

        inline sharpen::ByteBuffer Get(const sharpen::ByteBuffer &key) const
        {
            auto r{this->TryGet(key)};
            if(!r.Exist())
            {
                throw std::out_of_range("key doesn't exist");
            }
            return r.Get();
        }

        //for range query
        //you should lock level lock(S) first
        inline sharpen::AsyncReadWriteLock &GetLevelLock() const noexcept
        {
            return *this->levelLock_;
        }

        //for range query
        //you should lock level lock(S) first
        sharpen::Uint64 GetMaxLevel() const noexcept;

        //for range query
        //you should lock level lock(S) first
        inline std::shared_ptr<const sharpen::SortedStringTable> GetTable(sharpen::Uint64 id) const
        {
            return this->LoadTableCache(id);
        }

        inline sharpen::SortedStringTable GetTableCopy(sharpen::Uint64 id) const
        {
            return this->LoadTable(id);
        }

        //for range query
        //you should lock level lock(S) first
        template<typename _InsertIterator,typename _Check = decltype(*std::declval<_InsertIterator&>()++ = static_cast<const sharpen::LevelView*>(nullptr))>
        void GetAllViewOfComponent(sharpen::Uint64 level,_InsertIterator inserter) const
        {
            const sharpen::LevelComponent *component{&this->GetComponent(level)};
            for (auto begin = component->Begin(),end = component->End(); begin != end; ++begin)
            {
                *inserter++ = &this->GetView(*begin);   
            }
        }

        //for range query
        //you should lock level lock(S) first
        inline const MemTable &GetMemoryTable() const noexcept
        {
            return *this->mem_;
        }

        //for range query
        //you should lock level lock(S) first
        template<typename _InsertIterator,typename _Check = decltype(*std::declval<_InsertIterator&>()++ = static_cast<const MemTable*>(nullptr))>
        inline void GetAllImmutableTables(_InsertIterator inserter)
        {
            for (auto begin = this->imMems_.begin(),end = this->imMems_.end(); begin != end; ++begin)
            {
                *inserter++ = begin->get();
            }
        }

        sharpen::Uint64 GetTableSize() const;

        //for range query
        //you should lock level lock(S) first
        template<typename _InsertIterator,typename _Checker = decltype(*std::declval<_InsertIterator&>()++ = std::declval<sharpen::LevelViewItem&>())>
        inline void TableScan(_InsertIterator inserter) const
        {
            std::map<sharpen::Uint64,sharpen::Size> viewStatus;
            sharpen::Optional<sharpen::LevelViewItem> selectedItem;
            sharpen::Uint64 selectedView{0};
            sharpen::Uint64 maxLevel{this->GetMaxLevel()};
            std::set<sharpen::Uint64> emptyComponents;
            sharpen::Size levelBoundary{maxLevel + 1};
            while (emptyComponents.size() != levelBoundary)
            {
                for (sharpen::Size i = maxLevel;; --i)
                {
                    const sharpen::LevelComponent *component{&this->GetComponent(i)};
                    sharpen::Size emptyViews{0};
                    if(!component->Empty())
                    {
                        for (auto begin = component->Begin(),end = component->End(); begin != end; ++begin)
                        {
                            const sharpen::LevelView *view{&this->GetView(*begin)};
                            if(!view->Empty())
                            {
                                sharpen::Size skip{0};
                                auto ite = viewStatus.find(*begin);
                                if(ite != viewStatus.end())
                                {
                                    skip = ite->second;
                                }
                                else
                                {
                                    viewStatus.emplace(*begin,skip);
                                }
                                if(skip != view->GetSize())
                                {
                                    auto tableIte = sharpen::IteratorForward(view->Begin(),skip);
                                    if(!selectedItem.Exist() || this->CompareKeys(tableIte->BeginKey(),selectedItem.Get().BeginKey()) != -1)
                                    {
                                        selectedItem.Construct(*tableIte);
                                        selectedView = *begin;
                                    }
                                    continue;
                                }
                            }
                            emptyViews += 1;
                        }
                    }
                    if(emptyViews == component->GetSize())
                    {
                        emptyComponents.emplace(i);
                    }
                    if(!i)
                    {
                        break;
                    }
                }
                if(selectedItem.Exist())
                {
                    *inserter++ = std::move(selectedItem.Get());
                    selectedItem.Reset();
                    viewStatus[selectedView] += 1;
                    selectedView = 0;
                }
            }
        }

        //for range query
        //you should lock level lock(S) first
        template<typename _InsertIterator,typename _Checker = decltype(*std::declval<_InsertIterator&>()++ = std::declval<sharpen::LevelViewItem&>())>
        inline void TableScan(_InsertIterator inserter,const sharpen::ByteBuffer &beginKey,const sharpen::ByteBuffer &endKey) const
        {
            std::map<sharpen::Uint64,std::pair<sharpen::Size,sharpen::Size>> viewStatus;
            sharpen::Optional<sharpen::LevelViewItem> selectedItem;
            sharpen::Uint64 selectedView{0};
            sharpen::Uint64 maxLevel{this->GetMaxLevel()};
            std::set<sharpen::Uint64> emptyComponents;
            //set query range
            for (sharpen::Size i = 0,count = maxLevel + 1; i != count; ++i)
            {
                const sharpen::LevelComponent *component{&this->GetComponent(i)};
                if(!component->Empty())
                { 
                    for (auto begin = component->Begin(),end = component->End(); begin != end; ++begin)
                    {
                        const sharpen::LevelView *view{&this->GetView(*begin)};
                        if(!view->Empty())
                        {
                            auto tableBegin = view->Begin(),tableEnd = view->End();
                            sharpen::Size beginRange{0};
                            sharpen::Size endRange{0};
                            for (;tableBegin != tableEnd; ++tableBegin,++beginRange)
                            {
                                sharpen::Int32 r{this->CompareKeys(beginKey,tableBegin->EndKey())};
                                if(r != -1)
                                {
                                    break;
                                }
                            }
                            endRange = beginRange + 1;
                            for (;tableBegin != tableEnd; ++tableBegin,++endRange)
                            {
                                sharpen::Int32 r{this->CompareKeys(endKey,tableBegin->EndKey())};
                                if(r != -1)
                                {
                                    break;
                                }
                            }
                            viewStatus.emplace(*begin,std::pair<sharpen::Size,sharpen::Size>{beginRange,endRange});
                        }
                    }   
                }
            }
            sharpen::Size levelBoundary{maxLevel+1};
            while (emptyComponents.size() != levelBoundary)
            {
                for (sharpen::Size i = maxLevel;; --i)
                {
                    const sharpen::LevelComponent *component{&this->GetComponent(i)};
                    sharpen::Size emptyViews{0};
                    if(!component->Empty())
                    {
                        for (auto begin = component->Begin(),end = component->End(); begin != end; ++begin)
                        {
                            const sharpen::LevelView *view{&this->GetView(*begin)};
                            if(!view->Empty())
                            {
                                sharpen::Size skip{0};
                                auto ite = viewStatus.find(*begin);
                                assert(ite != viewStatus.end());
                                skip = ite->second.first;
                                if(skip != ite->second.second)
                                {
                                    auto tableIte = sharpen::IteratorForward(view->Begin(),skip);
                                    if(!selectedItem.Exist() || this->CompareKeys(tableIte->BeginKey(),selectedItem.Get().BeginKey()) != -1)
                                    {
                                        selectedItem.Construct(*tableIte);
                                        selectedView = *begin;
                                    }
                                    continue;
                                }
                            }
                            emptyViews += 1;
                        }
                    }
                    if(emptyViews == component->GetSize())
                    {
                        emptyComponents.emplace(i);
                    }
                    if(!i)
                    {
                        break;
                    }
                }
                if(selectedItem.Exist())
                {
                    *inserter++ = std::move(selectedItem.Get());
                    selectedItem.Reset();
                    viewStatus[selectedView].first += 1;
                    selectedView = 0;
                }
            }
        }

        void Destory();

        inline sharpen::LevelTableScanner Scan(bool useCache) const
        {
            sharpen::LevelTableScanner scanner{*this};
            if(!useCache)
            {
                scanner.DisableCache();
            }
            return scanner;
        }

        inline sharpen::LevelTableScanner Scan(const sharpen::ByteBuffer &beginKey,const sharpen::ByteBuffer &endKey,bool useCache) const
        {
            sharpen::LevelTableScanner scanner{*this,beginKey,endKey};
            if(!useCache)
            {
                scanner.DisableCache();
            }
            return scanner;
        }
    };
}

#endif