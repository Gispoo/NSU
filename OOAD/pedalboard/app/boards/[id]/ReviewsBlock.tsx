"use client";

import { useEffect, useState } from "react";
import { createClient } from "@/lib/supabase/client";

type Review = {
  id: string;
  user_id: string;
  content: string;
  rating: number | null;
  created_at: string;
};

export function ReviewsBlock({ pedalboardId }: { pedalboardId: string }) {
  const supabase = createClient();

  const [userId, setUserId] = useState<string | null>(null);
  const [reviews, setReviews] = useState<Review[]>([]);
  const [loading, setLoading] = useState(true);

  const [content, setContent] = useState("");
    const [rating, setRating] = useState<number | null>(null);
  const [hover, setHover] = useState<number | null>(null);

  const [err, setErr] = useState<string | null>(null);

  async function loadReviews() {
    setLoading(true);
    setErr(null);

    const { data, error } = await supabase
      .from("reviews")
      .select("id, user_id, content, rating, created_at")
      .eq("pedalboard_id", pedalboardId)
      .order("created_at", { ascending: false });

    if (error) setErr(error.message);
    setReviews((data ?? []) as Review[]);
    setLoading(false);
  }

  useEffect(() => {
    let cancelled = false;

    (async () => {
      const { data } = await supabase.auth.getUser();
      if (!cancelled) setUserId(data.user?.id ?? null);
    })();

    loadReviews();

    return () => {
      cancelled = true;
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [pedalboardId]);

async function submit() {
  setErr(null);

  const { data } = await supabase.auth.getUser();
  const user = data.user;

  if (!user) {
    const currentPath = window.location.pathname + window.location.search;
    window.location.href = `/auth/login?next=${encodeURIComponent(currentPath)}`;
    return;
  }

  if (!content.trim()) {
    setErr("Текст отзыва пустой");
    return;
  }

  // Проверяем что rating не null (пользователь поставил оценку)
  if (rating === null) { // было: if (rating === "")
    setErr("Поставь оценку");
    return;
  }

  const payload: any = {
    user_id: user.id,
    pedalboard_id: pedalboardId,
    content: content.trim(),
    rating: rating, // rating теперь всегда число
  };

  const { error } = await supabase.from("reviews").insert(payload);

  if (error) {
    setErr(error.message);
    return;
  }

  setContent("");
  setRating(null); // сбрасываем на null, а не ""
  await loadReviews();
}

  async function deleteReview(reviewId: string) {
    setErr(null);

    const { data } = await supabase.auth.getUser();
    const user = data.user;

    if (!user) {
        const currentPath = window.location.pathname + window.location.search;
        window.location.href = `/auth/login?next=${encodeURIComponent(currentPath)}`;
        return;
    }

    const ok = confirm("Удалить отзыв?");
    if (!ok) return;

    const { error } = await supabase.from("reviews").delete().eq("id", reviewId);

    if (error) {
        setErr(error.message);
        return;
    }

    await loadReviews();
    }

      // Средний рейтинг
  const rated = reviews.filter((r) => typeof r.rating === "number") as Array<
    Review & { rating: number }
  >;
  const avg =
    rated.length === 0
      ? null
      : rated.reduce((sum, r) => sum + r.rating, 0) / rated.length;

  const avgRounded = avg == null ? null : Math.round(avg * 10) / 10; // 4.3
  const avgStars = avg == null ? 0 : Math.round(avg); // для "★★★★☆"

  return (
    <section style={{ marginTop: 18, display: "grid", gap: 12 }}>
      <h2 style={{ display: "flex", alignItems: "center", gap: 12 }}>
        Отзывы
        {!loading && avgRounded != null && (
            <span style={{ fontSize: 14, opacity: 0.85 }}>
            {avgRounded} / 5 • {rated.length} оценок
            {" "}
            <span style={{ marginLeft: 8 }}>
                {"★".repeat(avgStars)}
                {"☆".repeat(5 - avgStars)}
            </span>
            </span>
        )}
        </h2>

      {userId ? (
        <div style={{ display: "grid", gap: 8, border: "1px solid #ddd", padding: 12, borderRadius: 10 }}>
            <div style={{ display: "flex", gap: 6 }}>
                {[1, 2, 3, 4, 5].map((star) => {
                    // Приводим к числу, null/"" станет 0
                    const currentRating = hover ?? rating ?? 0;
                    const isActive = currentRating >= star;
                    
                    return (
                    <span
                        key={star}
                        onMouseEnter={() => setHover(star)}
                        onMouseLeave={() => setHover(null)}
                        onClick={() => setRating(star)}
                        style={{
                        cursor: "pointer",
                        fontSize: 28,
                        color: isActive ? "#FFD700" : "#ccc",
                        }}
                    >
                        ★
                    </span>
                    );
                })}
                </div>

          <textarea
            placeholder="Напиши отзыв..."
            value={content}
            onChange={(e) => setContent(e.target.value)}
            rows={3}
            style={{ padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
          />

          <button onClick={submit} style={{ padding: 10, borderRadius: 10, width: "fit-content" }}>
            Отправить
          </button>

          {err && <div style={{ color: "crimson" }}>{err}</div>}
        </div>
      ) : (
        <div style={{ opacity: 0.8 }}>
          Чтобы оставить отзыв, нужно <a href="/auth/login">войти</a>.
        </div>
      )}

      {loading ? (
        <div>Загрузка отзывов...</div>
      ) : reviews.length === 0 ? (
        <div>Пока нет отзывов.</div>
      ) : (
        <div style={{ display: "grid", gap: 8 }}>
          {reviews.map((r) => (
            <div key={r.id} style={{ border: "1px solid #ddd", padding: 12, borderRadius: 10 }}>
                <div style={{ display: "flex", justifyContent: "space-between", gap: 12 }}>
                <div style={{ fontSize: 12, opacity: 0.7 }}>
                    {new Date(r.created_at).toLocaleString()} {r.rating != null ? `• ⭐ ${r.rating}` : ""}
                </div>

                {userId && userId === r.user_id && (
                    <button
                    onClick={() => deleteReview(r.id)}
                    style={{
                        padding: "4px 8px",
                        borderRadius: 8,
                        border: "1px solid #ddd",
                        background: "#f44336",
                        color: "white",
                        cursor: "pointer",
                        fontSize: 12,
                    }}
                    >
                    Удалить
                    </button>
                )}
                </div>

                <div style={{ marginTop: 6 }}>{r.content}</div>
            </div>
            ))}

        </div>
      )}
    </section>
  );
}
