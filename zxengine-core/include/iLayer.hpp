#pragma once
#include "id_counter.hpp"
#include <set>
namespace zx
{
	using layer_id = int;
	class iLayer
	{
	public:
		iLayer(layer_id layer = 0) noexcept
			: pm_Enabled(true), pm_Layer(layer) {}

		[[nodiscard]] inline layer_id layer() const noexcept { return pm_Layer; }
		[[nodiscard]] inline layer_id& layer() noexcept { return pm_Layer; }

		[[nodiscard]] inline bool enabled() const noexcept { return pm_Enabled; }

		void Enable()
		{
			if (!pm_Enabled)
				return;

			pm_Enabled = true;
			LayerEnabled();
		}
		void Disable()
		{
			if (pm_Enabled)
				return;
			pm_Enabled = false;
			LayerDisbled();
		}
		virtual void LayerAdded() {}
		virtual void LayerRemoved() {}
		virtual void LayerEnabled() {}
		virtual void LayerDisbled() {}
	private:
		layer_id pm_Layer;
		bool pm_Enabled;
	};



	template<std::derived_from<iLayer> T = iLayer>
	class LayerGroup
	{
	protected:
		struct comparator
		{
			bool operator()(T* lhs, T* rhs) const { return lhs->layer() < rhs->layer(); }
		};
	public:
		using set = std::multiset<T*, comparator>;
		using iterator = set::iterator;
		using const_iterator = set::const_iterator;
		using reverse_iterator = set::reverse_iterator;
		using const_reverse_iterator = set::const_reverse_iterator;

		using layer_type = T;

		template<typename TFunc>
		requires requires(TFunc f, T* l) { {f(l)} -> std::same_as<void>; }
		void foreach(TFunc& f) {
			for (auto layer : pm_Layers)
				f(layer);
		}
		template<typename TFunc>
		requires requires(TFunc f, T* l) { {f(l)} -> std::same_as<void>; }
		void foreach(TFunc&& f) {
			for (auto layer : pm_Layers)
				f(layer);
		}

		zxIteratorFunctions(pm_Layers);

		iterator Add(T* value) noexcept {
			auto it = pm_Layers.insert(value);
			LayerAdded(value);
			return it;
		}
		bool Remove(T* value) noexcept
		{
			auto it = std::find(begin(), end(), value);
			if (it == end())
				return false;
			pm_Layers.erase(it);

			LayerRemoved(value);
			return true;
		}

		void Sort()
		{
			std::vector<T*> list;
			list.reserve(pm_Layers.size());

			for (auto layer : pm_Layers)
				list.push_back(layer);
			pm_Layers.clear();
			for (auto layer : list)
				pm_Layers.insert(layer);
		}
	protected:
		virtual void LayerAdded(T* layer) {}
		virtual void LayerRemoved(T* layer) {}
		set pm_Layers;
	};
}