"use client";

import { useState } from "react";
import { createClient } from "@/lib/supabase/client";

function pathFromPublicUrl(url: string) {
  // пример:
  // https://.../storage/v1/object/public/pedalboard-images/<PATH>
  const marker = "/storage/v1/object/public/";
  const i = url.indexOf(marker);
  if (i === -1) return null;

  const rest = url.slice(i + marker.length); // bucketId/<path>
  const slash = rest.indexOf("/");
  if (slash === -1) return null;

  const bucketId = rest.slice(0, slash);
  const path = rest.slice(slash + 1);
  return { bucketId, path };
}

export function DeleteButton({
  boardId,
  imageUrl,
  audioUrl,
}: {
  boardId: string;
  imageUrl: string | null;
  audioUrl: string | null;
}) {
  const supabase = createClient();
  const [loading, setLoading] = useState(false);
  const [err, setErr] = useState<string | null>(null);

  async function onDelete() {
    const ok = confirm("Удалить педалборд? Это действие нельзя отменить.");
    if (!ok) return;

    setErr(null);
    setLoading(true);

    try {
      // 1) удалить файлы из storage (если есть)
      const toRemove: { bucketId: string; path: string }[] = [];

      if (imageUrl) {
        const p = pathFromPublicUrl(imageUrl);
        if (p) toRemove.push(p);
      }
      if (audioUrl) {
        const p = pathFromPublicUrl(audioUrl);
        if (p) toRemove.push(p);
      }

      // сгруппируем по bucket
      const byBucket = new Map<string, string[]>();
      for (const f of toRemove) {
        byBucket.set(f.bucketId, [...(byBucket.get(f.bucketId) ?? []), f.path]);
      }

      for (const [bucketId, paths] of byBucket.entries()) {
        const { error } = await supabase.storage.from(bucketId).remove(paths);
        // Если файла уже нет — не критично, но ошибку покажем
        if (error) {
          setErr(`Storage delete (${bucketId}): ${error.message}`);
          setLoading(false);
          return;
        }
      }

      // 2) удалить запись педалборда (favorites/reviews удалятся каскадом)
      const { error: delErr } = await supabase
        .from("pedalboards")
        .delete()
        .eq("id", boardId);

      if (delErr) {
        setErr(delErr.message);
        setLoading(false);
        return;
      }

      // 3) домой
      window.location.href = "/";
    } catch (e: any) {
      setErr(e?.message ?? String(e));
    } finally {
      setLoading(false);
    }
  }

  return (
    <div style={{ display: "grid", gap: 8 }}>
      <button
        onClick={onDelete}
        disabled={loading}
        style={{
          padding: "8px 12px",
          borderRadius: 8,
          border: "1px solid #ddd",
          background: "#f44336",
          color: "white",
          width: "fit-content",
        }}
      >
        {loading ? "Удаляю..." : "Удалить педалборд"}
      </button>

      {err && <div style={{ color: "crimson" }}>{err}</div>}
    </div>
  );
}
