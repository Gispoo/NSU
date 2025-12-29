"use client";

import { useState, useEffect } from "react"; // ← добавь useEffect
import { createClient } from "@/lib/supabase/client";

export default function NewBoardPage() {
  const supabase = createClient();

  const [title, setTitle] = useState("");
  const [shortDesc, setShortDesc] = useState("");
  const [fullDesc, setFullDesc] = useState("");
  const [imageFile, setImageFile] = useState<File | null>(null);
  const [audioFile, setAudioFile] = useState<File | null>(null);

  const [err, setErr] = useState<string | null>(null);
  const [ok, setOk] = useState<string | null>(null);
  const [saving, setSaving] = useState(false);

  // === ДОБАВЬ ЭТОТ useEffect ДЛЯ ВОССТАНОВЛЕНИЯ ДАННЫХ ===
  useEffect(() => {
    // Проверяем есть ли сохранённые данные после логина
    const savedData = localStorage.getItem('pending_pedalboard');
    if (savedData) {
      try {
        const formData = JSON.parse(savedData);
        setTitle(formData.title || "");
        setShortDesc(formData.shortDesc || "");
        setFullDesc(formData.fullDesc || "");
        
        // Очищаем localStorage
        localStorage.removeItem('pending_pedalboard');
        
        // Можно показать сообщение
        setOk("Данные формы восстановлены!");
      } catch (error) {
        console.error("Ошибка восстановления данных:", error);
      }
    }
  }, []);
  // === КОНЕЦ ===

  async function submit() {
    setErr(null);
    setOk(null);

    const { data } = await supabase.auth.getUser();
    const user = data.user;

    if (!user) {
      // === ВСТАВЬ ЭТО ВМЕСТО ПРОСТОГО РЕДИРЕКТА ===
      // Сохраняем данные формы
      const formData = {
        title,
        shortDesc,
        fullDesc,
        // Файлы не сохраняем в localStorage
      };
      localStorage.setItem('pending_pedalboard', JSON.stringify(formData));
      
      // Редирект на логин с возвратом на эту страницу
      const currentPath = window.location.pathname;
      window.location.href = `/auth/login?next=${encodeURIComponent(currentPath)}`;
      // === КОНЕЦ ===
      return;
    }

    if (!title.trim()) {
      setErr("Название обязательно");
      return;
    }

    setSaving(true);

    try {
      // 1) Загружаем файлы в Storage (если выбраны)
      let imageUrl: string | null = null;
      let audioUrl: string | null = null;

      if (imageFile) {
        const ext = imageFile.name.split(".").pop() || "jpg";
        const filePath = `${user.id}/${crypto.randomUUID()}.${ext}`;

        const { error: upErr } = await supabase.storage
          .from("pedalboard-images")
          .upload(filePath, imageFile, { contentType: imageFile.type });

        if (upErr) {
          setErr("Image upload: " + upErr.message);
          setSaving(false);
          return;
        }

        const { data: pub } = supabase.storage
          .from("pedalboard-images")
          .getPublicUrl(filePath);

        imageUrl = pub.publicUrl;
      }

      if (audioFile) {
        const ext = audioFile.name.split(".").pop() || "mp3";
        const filePath = `${user.id}/${crypto.randomUUID()}.${ext}`;

        const { error: upErr } = await supabase.storage
          .from("pedalboard-audio")
          .upload(filePath, audioFile, { contentType: audioFile.type });

        if (upErr) {
          setErr("Audio upload: " + upErr.message);
          setSaving(false);
          return;
        }

        const { data: pub } = supabase.storage
          .from("pedalboard-audio")
          .getPublicUrl(filePath);

        audioUrl = pub.publicUrl;
      }

      // 2) Создаём запись в БД, сохраняя public URL
      const { data: inserted, error } = await supabase
        .from("pedalboards")
        .insert({
          user_id: user.id,
          title: title.trim(),
          short_desc: shortDesc.trim() || null,
          full_desc: fullDesc.trim() || null,
          image_url: imageUrl,
          audio_url: audioUrl,
        })
        .select("id")
        .single();

      setSaving(false);

      if (error) {
        setErr(error.message);
        return;
      }

      setOk("Создано!");
      window.location.href = `/boards/${inserted.id}`;
    } catch (e: any) {
      setSaving(false);
      setErr(e?.message ?? String(e));
    }
  }

  return (
    <main style={{ padding: 24, display: "grid", gap: 12, maxWidth: 720 }}>
      <h1>Создать педалборд</h1>

      <label>
        Название *
        <input
          value={title}
          onChange={(e) => setTitle(e.target.value)}
          style={{ width: "100%", padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
        />
      </label>

      <label>
        Короткое описание
        <input
          value={shortDesc}
          onChange={(e) => setShortDesc(e.target.value)}
          style={{ width: "100%", padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
        />
      </label>

      <label>
        Полное описание
        <textarea
          value={fullDesc}
          onChange={(e) => setFullDesc(e.target.value)}
          rows={5}
          style={{ width: "100%", padding: 10, borderRadius: 8, border: "1px solid #ccc" }}
        />
      </label>

      <label>
        Фото педалборда
        <input
          type="file"
          accept="image/*"
          onChange={(e) => setImageFile(e.target.files?.[0] ?? null)}
        />
      </label>

      <label>
        Аудио (пример звучания)
        <input
          type="file"
          accept="audio/*"
          onChange={(e) => setAudioFile(e.target.files?.[0] ?? null)}
        />
      </label>

      <button
        onClick={submit}
        disabled={saving}
        style={{ padding: 10, borderRadius: 10, width: "fit-content" }}
      >
        {saving ? "Сохраняю..." : "Создать"}
      </button>

      {err && <div style={{ color: "crimson" }}>{err}</div>}
      {ok && <div style={{ color: "green" }}>{ok}</div>}
    </main>
  );
}