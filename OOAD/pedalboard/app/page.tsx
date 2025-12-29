"use client";

import { useEffect, useMemo, useState } from "react";
import { createClient } from "@/lib/supabase/client"; // ← используй КЛИЕНТСКИЙ клиент!

type Board = {
  id: string;
  title: string;
  short_desc: string | null;
  image_url: string | null;
  created_at: string;
  avg_rating: number | null;
  reviews_count: number | null;
};

type SortMode = "new" | "rating";

export default function Page() {
  const supabase = createClient(); // ← ТАК ПРАВИЛЬНО (client-side)

  const [q, setQ] = useState("");
  const [minRating, setMinRating] = useState<number>(0);
  const [sort, setSort] = useState<SortMode>("new");

  const [loading, setLoading] = useState(true);
  const [err, setErr] = useState<string | null>(null);
  const [data, setData] = useState<Board[]>([]);

  async function load() {
    setLoading(true);
    setErr(null);

    let query = supabase
      .from("pedalboards_with_rating") // ← Убедись что эта вьюха существует!
      .select("id, title, short_desc, image_url, created_at, avg_rating, reviews_count");

    // поиск
    const qq = q.trim();
    if (qq.length > 0) {
      query = query.or(`title.ilike.%${qq}%,short_desc.ilike.%${qq}%`);
    }

    // фильтр по рейтингу
    if (minRating > 0) {
      query = query.gte("avg_rating", minRating);
    }

    // сортировка
    if (sort === "rating") {
      query = query.order("avg_rating", { ascending: false, nullsFirst: false });
      query = query.order("reviews_count", { ascending: false, nullsFirst: false });
      query = query.order("created_at", { ascending: false });
    } else {
      query = query.order("created_at", { ascending: false });
    }

    query = query.limit(50);

    const { data, error } = await query;

    if (error) {
      console.error("Ошибка загрузки:", error);
      setErr(error.message);
      setData([]);
    } else {
      setData((data ?? []) as Board[]);
    }
    setLoading(false);
  }

  // debounce
  useEffect(() => {
    const t = setTimeout(() => {
      load();
    }, 250);
    return () => clearTimeout(t);
  }, [q, minRating, sort]);

  const subtitle = useMemo(() => {
    const parts: string[] = [];
    if (q.trim()) parts.push(`поиск: "${q.trim()}"`);
    if (minRating > 0) parts.push(`рейтинг ≥ ${minRating}`);
    parts.push(sort === "new" ? "сорт: новые" : "сорт: рейтинг");
    return parts.join(" • ");
  }, [q, minRating, sort]);

  return (
    <main className="container">
      <h1 style={{ marginBottom: 8 }}>Pedalboards</h1>
      <div style={{ opacity: 0.75, marginBottom: 16 }}>{subtitle}</div>

      {/* Панель поиска/фильтров */}
      <div
        style={{
          display: "grid",
          gridTemplateColumns: "1fr 180px 180px 120px",
          gap: 10,
          alignItems: "end",
          marginBottom: 20,
        }}
      >
        <div style={{ display: "grid", gap: 4 }}>
          <div style={{ fontSize: 12, opacity: 0.7 }}>Поиск</div>
          <input
            value={q}
            onChange={(e) => setQ(e.target.value)}
            placeholder="Название или описание..."
            style={{ padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
          />
        </div>

        <div style={{ display: "grid", gap: 4 }}>
          <div style={{ fontSize: 12, opacity: 0.7 }}>Мин. рейтинг</div>
          <select
            value={minRating}
            onChange={(e) => setMinRating(Number(e.target.value))}
            style={{ padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
          >
            <option value={0}>Любой</option>
            <option value={1}>1+ ⭐</option>
            <option value={2}>2+ ⭐⭐</option> 
            <option value={3}>3+ ⭐⭐⭐</option>
            <option value={4}>4+ ⭐⭐⭐⭐</option>
            <option value={5}>5 ⭐⭐⭐⭐⭐</option>
          </select>
        </div>

        <div style={{ display: "grid", gap: 4 }}>
          <div style={{ fontSize: 12, opacity: 0.7 }}>Сортировка</div>
          <select
            value={sort}
            onChange={(e) => setSort(e.target.value as SortMode)}
            style={{ padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
          >
            <option value="new">🆕 Сначала новые</option>
            <option value="rating">⭐ По рейтингу</option>
          </select>
        </div>

        <button
          onClick={() => {
            setQ("");
            setMinRating(0);
            setSort("new");
          }}
          style={{
            padding: 10,
            borderRadius: 8,
            border: "1px solid #ddd",
            background: "#f5f5f5",
            cursor: "pointer",
            fontWeight: 500,
          }}
        >
          Сбросить фильтры
        </button>
      </div>

      {/* Список */}
      {loading ? (
        <div style={{ padding: 40, textAlign: "center", opacity: 0.7 }}>
          Загрузка...
        </div>
      ) : err ? (
        <div style={{ 
          padding: 20, 
          background: "#ffebee", 
          borderRadius: 8, 
          color: "#c62828",
          border: "1px solid #ffcdd2"
        }}>
          <strong>Ошибка:</strong> {err}
          <br />
          <small style={{ opacity: 0.8 }}>
            Проверьте что вьюха "pedalboards_with_rating" существует
          </small>
        </div>
      ) : data.length === 0 ? (
        <div style={{ padding: 40, textAlign: "center", opacity: 0.7 }}>
          Ничего не найдено. Попробуйте изменить параметры поиска.
        </div>
      ) : (
        <div style={{ display: "grid", gap: 12 }}>
          {data.map((b) => (
            <a
              key={b.id}
              href={`/boards/${b.id}`}
              style={{
                display: "flex",
                gap: 16,
                border: "1px solid #e0e0e0",
                padding: 16,
                borderRadius: 12,
                textDecoration: "none",
                color: "inherit",
                background: "white",
                boxShadow: "0 2px 4px rgba(0,0,0,0.05)",
                transition: "transform 0.2s, box-shadow 0.2s",
              }}
              onMouseEnter={(e) => {
                e.currentTarget.style.transform = "translateY(-2px)";
                e.currentTarget.style.boxShadow = "0 4px 8px rgba(0,0,0,0.1)";
              }}
              onMouseLeave={(e) => {
                e.currentTarget.style.transform = "translateY(0)";
                e.currentTarget.style.boxShadow = "0 2px 4px rgba(0,0,0,0.05)";
              }}
            >
              {b.image_url ? (
                <img
                  src={b.image_url}
                  alt={b.title}
                  style={{ 
                    width: 100, 
                    height: 100, 
                    objectFit: "cover", 
                    borderRadius: 10,
                    flexShrink: 0 
                  }}
                />
              ) : (
                <div style={{ 
                  width: 100, 
                  height: 100, 
                  borderRadius: 10, 
                  background: "#f5f5f5",
                  display: "flex",
                  alignItems: "center",
                  justifyContent: "center",
                  color: "#999",
                  fontSize: 12
                }}>
                  No image
                </div>
              )}

              <div style={{ flex: 1 }}>
                <div style={{ 
                  fontWeight: 700, 
                  fontSize: 18, 
                  marginBottom: 8,
                  color: "#333"
                }}>
                  {b.title}
                </div>

                {b.avg_rating != null && b.avg_rating > 0 ? (
                  <div style={{ 
                    fontSize: 14, 
                    color: "#666",
                    marginBottom: 8,
                    display: "flex",
                    alignItems: "center",
                    gap: 8
                  }}>
                    <span style={{ 
                      background: "#4caf50", 
                      color: "white", 
                      padding: "2px 6px", 
                      borderRadius: 4,
                      fontWeight: 600
                    }}>
                      {b.avg_rating.toFixed(1)} ⭐
                    </span>
                    <span>
                      {b.reviews_count} оценок
                    </span>
                    <span style={{ marginLeft: 8 }}>
                      {"★".repeat(Math.round(b.avg_rating))}
                      {"☆".repeat(5 - Math.round(b.avg_rating))}
                    </span>
                  </div>
                ) : (
                  <div style={{ 
                    fontSize: 14, 
                    color: "#999",
                    marginBottom: 8
                  }}>
                    Пока нет оценок
                  </div>
                )}

                <div style={{ 
                  color: "#666", 
                  fontSize: 15,
                  lineHeight: 1.4
                }}>
                  {b.short_desc}
                </div>
              </div>
            </a>
          ))}
        </div>
      )}
    </main>
  );
}