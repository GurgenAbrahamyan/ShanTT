#include "RenderGraph.h"
#include <unordered_set>
#include <deque>
#include <sstream>
#include <unordered_map>

bool RenderGraph::compile(std::vector<std::string>& outErrors)
{
    executionOrder.clear();
    outErrors.clear();

    std::unordered_map<PassId, RenderPass*> byId;
    for (auto& p : passes) byId[p->id] = p.get();


    std::unordered_map<ResourceId, RenderPass*> writerOf;
    std::unordered_map<ResourceId, std::vector<RenderPass*>> extraWritersOf; 

    for (auto& p : passes)
    {
        for (ResourceId out : p->outputs)
        {
            auto it = writerOf.find(out);
            if (it == writerOf.end())
                writerOf[out] = p.get();
            else
                extraWritersOf[out].push_back(p.get());
        }
    }

    std::unordered_map<RenderPass*, std::vector<RenderPass*>> adjacency; 
    std::unordered_map<RenderPass*, int> inDegree;
    for (auto& p : passes) inDegree[p.get()] = 0;

    auto addEdge = [&](RenderPass* from, RenderPass* to)
    {
        adjacency[from].push_back(to);
        inDegree[to]++;
    };

    for (auto& p : passes)
    {
        for (ResourceId in : p->inputs)
        {
            auto it = writerOf.find(in);
            if (it == writerOf.end())
            {
                outErrors.push_back("Pass '" + p->name + "' requires resource "
                    + std::to_string(static_cast<uint32_t>(in))
                    + " but no enabled pass produces it.");
                continue;
            }
            if (it->second != p.get())
                addEdge(it->second, p.get());
        }
    }


    for (auto& [resId, extras] : extraWritersOf)
    {
        RenderPass* firstWriter = writerOf[resId];
        for (RenderPass* extra : extras)
        {
            bool ordered = false;
            for (PassId dep : extra->orderAfter)
            {
                auto it = byId.find(dep);
                if (it != byId.end() && it->second == firstWriter) { ordered = true; break; }
            }
            if (!ordered)
            {
                outErrors.push_back("Resource " + std::to_string(static_cast<uint32_t>(resId))
                    + " has multiple writers ('" + firstWriter->name + "', '" + extra->name
                    + "') with no declared order. Add orderAfter().");
            }
            else
            {
                addEdge(firstWriter, extra);
            }
        }
    }

    for (auto& p : passes)
    {
        for (PassId dep : p->orderAfter)
        {
            auto it = byId.find(dep);
            if (it == byId.end())
            {
                outErrors.push_back("Pass '" + p->name + "' has orderAfter referencing unknown pass id "
                    + std::to_string(static_cast<uint32_t>(dep)) + ".");
                continue;
            }
            addEdge(it->second, p.get());
        }
    }

    if (!outErrors.empty())
        return false;

    std::deque<RenderPass*> ready;
    for (auto& p : passes)
        if (inDegree[p.get()] == 0)
            ready.push_back(p.get());

    std::unordered_set<RenderPass*> visited;
    while (!ready.empty())
    {
        RenderPass* current = ready.front();
        ready.pop_front();
        executionOrder.push_back(current);
        visited.insert(current);

        for (RenderPass* next : adjacency[current])
        {
            if (--inDegree[next] == 0)
                ready.push_back(next);
        }
    }

    if (executionOrder.size() != passes.size())
    {
        std::ostringstream oss;
        oss << "Cycle detected involving passes: ";
        for (auto& p : passes)
            if (!visited.count(p.get()))
                oss << "'" << p->name << "' ";
        outErrors.push_back(oss.str());
        executionOrder.clear();
        return false;
    }

    dirty = false;
    return true;
}