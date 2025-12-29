"use client";

import { useParams } from "next/navigation";
import { useEffect, useState } from "react";
import { createClient } from "@/lib/supabase/client";
import { FavoriteButton } from "./FavoriteButton";
import { ReviewsBlock } from "./ReviewsBlock";
import { DeleteButton } from "./DeleteButton";

export default function BoardPage() {
  const params = useParams<{ id: string }>();
  const id = params?.id;

  const [board, setBoard] = useState<any>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  const [isAuthed, setIsAuthed] = useState(false);
  const [initialIsFav, setInitialIsFav] = useState(false);
  const [user, setUser] = useState<any>(null); // <-- ДОБАВИТЬ ЭТУ СТРОКУ

  useEffect(() => {
    let cancelled = false;

    async function load() {
      if (!id) return;

      setLoading(true);
      setError(null);

      const supabase = createClient();

      // 1) board
      const { data: b, error: e1 } = await supabase
        .from("pedalboards")
        .select("*")
        .eq("id", id)
        .single();

      if (e1) {
        console.log("pedalboards error:", e1);
      }

      if (cancelled) return;

      if (e1) {
        setError(e1.message);
        setLoading(false);
        return;
      }

      setBoard(b);

      // 2) user + favorite status
      const { data: userData } = await supabase.auth.getUser();
      const currentUser = userData.user;
      setUser(currentUser); // <-- СОХРАНЯЕМ ПОЛЬЗОВАТЕЛЯ
      setIsAuthed(!!currentUser);

      if (currentUser) {
        const { data: fav } = await supabase
          .from("favorites")
          .select("pedalboard_id")
          .eq("user_id", currentUser.id)
          .eq("pedalboard_id", id)
          .maybeSingle();

        if (!cancelled) setInitialIsFav(!!fav);
      }

      if (!cancelled) setLoading(false);
    }

    load();

    return () => {
      cancelled = true;
    };
  }, [id]);

  if (!id) {
    return (
      <main style={{ padding: 24 }}>
        <h1>Ошибка</h1>
        <pre>id is empty</pre>
      </main>
    );
  }

  if (loading) {
    return <main style={{ padding: 24 }}>Загрузка...</main>;
  }

  if (error) {
    return (
      <main style={{ padding: 24 }}>
        <h1>Не найдено / ошибка</h1>
        <pre>{error}</pre>
      </main>
    );
  }

  // Теперь user доступен
  const isOwner = user && board.user_id === user.id;

  return (
    <main style={{ padding: 24, display: "grid", gap: 12 }}>
      <h1>{board.title}</h1>

      {/* Показываем DeleteButton только владельцу */}
      {isOwner && (
        <DeleteButton 
          boardId={id} 
          imageUrl={board.image_url}
          audioUrl={board.audio_url}
        />
      )}

      <FavoriteButton
        pedalboardId={id}
        initialIsFav={initialIsFav}
        isAuthed={isAuthed}
      />

      {board.image_url && (
        <>
          <img
            src={board.image_url}
            alt={board.title}
            style={{ maxWidth: 400, borderRadius: 12 }}
          />
        </>
      )}

      <p>{board.full_desc ?? board.short_desc}</p>

      {board.audio_url && <audio controls src={board.audio_url} />}

      <ReviewsBlock pedalboardId={id} />
    </main>
  );
}