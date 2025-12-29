"use client";

import { useState } from "react";
import { createClient } from "@/lib/supabase/client";

export function FavoriteButton({
  pedalboardId,
  initialIsFav,
  isAuthed,
}: {
  pedalboardId: string;
  initialIsFav: boolean;
  isAuthed: boolean;
}) {
  const [isFav, setIsFav] = useState(initialIsFav);
  const [loading, setLoading] = useState(false);

  async function toggle() {
    if (!isAuthed) {
      const currentPath = window.location.pathname;
      window.location.href = `/auth/login?next=${encodeURIComponent(currentPath)}`;
      return;
    }

    setLoading(true);
    const supabase = createClient();

    if (!isFav) {
      // ВАЖНО: тут мы вставляем запись в favorites
      const { data: userData } = await supabase.auth.getUser();
      const user = userData.user;
      if (!user) {
        const currentPath = window.location.pathname;
        window.location.href = `/auth/login?next=${encodeURIComponent(currentPath)}`;
        return;
      }

      const { error } = await supabase.from("favorites").insert({
        user_id: user.id,
        pedalboard_id: pedalboardId,
      });

      if (error) alert(error.message);
      else setIsFav(true);
    } else {
      const { data: userData } = await supabase.auth.getUser();
      const user = userData.user;
      if (!user) {
        const currentPath = window.location.pathname;
        window.location.href = `/auth/login?next=${encodeURIComponent(currentPath)}`;
        return;
      }

      const { error } = await supabase
        .from("favorites")
        .delete()
        .eq("user_id", user.id)
        .eq("pedalboard_id", pedalboardId);

      if (error) alert(error.message);
      else setIsFav(false);
    }

    setLoading(false);
  }

  return (
    <button
      onClick={toggle}
      disabled={loading}
      style={{ padding: 10, borderRadius: 10, width: "fit-content" }}
    >
      {isFav ? "💔 Убрать из избранного" : "❤️ В избранное"}
    </button>
  );
}
